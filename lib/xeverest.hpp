/*--------------------------------------------------------------------------*/
/*  Copyright 2020 Sergei Vostokin                                          */
/*                                                                          */
/*  Licensed under the Apache License, Version 2.0 (the "License");         */
/*  you may not use this file except in compliance with the License.        */
/*  You may obtain a copy of the License at                                 */
/*                                                                          */
/*  http://www.apache.org/licenses/LICENSE-2.0                              */
/*                                                                          */
/*  Unless required by applicable law or agreed to in writing, software     */
/*  distributed under the License is distributed on an "AS IS" BASIS,       */
/*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*/
/*  See the License for the specific language governing permissions and     */
/*  limitations under the License.                                          */
/*--------------------------------------------------------------------------*/
#pragma once

#include <curl/curl.h>

#ifndef NLOHMANN_JSON_VERSION_MAJOR
#include "json.hpp"
#endif

#include <cassert>
#include <functional>
#include <string>
#include <list>
#include <chrono>
#include <thread>
#include <iostream>

#include <stdio.h>
#include <sys/stat.h>

#include "xtemplet.hpp"

using json = nlohmann::json;
using namespace std;

const string EVEREST_URL = "https://everest.distcomp.org";
const std::chrono::seconds DELAY(1);
const bool VERBOSE = false;

namespace templet {

	class everest_task;
	
	size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
		((string *)userp)->append((char *)contents, size * nmemb);
		return size * nmemb;
	}

	struct event{
		string _state;
		string _id;
		everest_task*  _task;
	};

	class everest_engine {
		friend class everest_task;
	public:
		everest_engine(const char*login, const char*pass, const char* label=0) {
			_login = login; _pass = pass; _curl = NULL; _recount = 0;
			if (label)_label = label; else _label = "templet-session";
			_is_ready = init();
		}
		everest_engine(const char* access_token) {
			_curl = NULL; _recount = 0; _access_token = access_token;
			_is_ready = init();
		}

		~everest_engine() {	cleanup(); }
		void free_access_token(){_pass.clear();}
		bool ready() { return _is_ready; }

		bool wait_all();
		bool wait_for(everest_task& t);

		bool print_app_description(const char* app_name);
		bool get_access_token(string&t) {if(_is_ready){t=_access_token; return true;} else return false;}

		bool upload(const string& file, string& uri);
		bool download(const string& file, const string& uri);
		bool remove(const string& uri);

	private:
		bool submit(everest_task&t);
		bool is_idle(everest_task&t);
		bool is_done(everest_task&t);

	private:
		inline bool _wait_loop_body(event&ev);
		inline bool init();
		inline void cleanup();

	private:
		string _login;
		string _pass;
		string _label;
		string _access_token;
		bool   _is_ready;
		int    _recount;

		curl_slist* _common_headers;
		curl_slist* _upload_headers;
		
		CURL*  _curl;
		long   _code;
		string _response;

		list<event> _submitted;
	};

	class everest_task : task {
		friend	class everest_engine;
	public:
		everest_task(everest_engine&e, const char* app_id=0) :
			_eng(&e), _actor(0), _tsk_adaptor(0), _is_idle(true), _is_done(false), _app_ID(app_id), _deletable(false) {}
		everest_task(actor*a, task_adaptor ta) : _actor(a), _tsk_adaptor(ta), _eng(0), _is_idle(true), _is_done(false),  _app_ID(""), _deletable(false) {}
		everest_task() : _actor(0), _tsk_adaptor(0), _eng(0), _is_idle(true), _is_done(false), _app_ID(""), _deletable(false) {}

		bool app_id(const char*id) { if (_is_idle) { _app_ID = id; return true; } return false; }
		bool engine(everest_engine&e) { if (_is_idle) { _eng = &e; return true; } return false; }
		void deletable(bool del) { _deletable = del; }
		
		json& output() { return _output; }
		void  input(json&in) { _input = in; }

		bool submit() { if(_actor)_actor->suspend(); return _eng->submit(*this); }
		bool submit(json& in) { if (_actor)_actor->suspend(); _input = in; return _eng->submit(*this); }

		bool idle() { return _is_idle; }
		bool done() { return _is_done; }

	private:
		json   _input;
		json   _output;
		string _app_ID;

		task_adaptor   _tsk_adaptor;
		actor*         _actor;
		everest_engine*_eng;
		
		bool _is_idle;
		bool _is_done;
		bool _deletable;
	};

	bool everest_engine::is_idle(everest_task&t) { return t._is_idle; }
	bool everest_engine::is_done(everest_task&t) { return t._is_done; }

	bool everest_engine::init() {
		curl_global_init(CURL_GLOBAL_ALL);

		_curl = curl_easy_init();
		if (!_curl) return false;

		curl_easy_setopt(_curl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(_curl, CURLOPT_VERBOSE, VERBOSE);

		_common_headers = NULL;
		_common_headers = curl_slist_append(_common_headers, "Accept: application/json");
		_common_headers = curl_slist_append(_common_headers, "Content-Type: application/json");
		_common_headers = curl_slist_append(_common_headers, "charsets: utf-8");

		_upload_headers = NULL;
		//_upload_headers = curl_slist_append(_upload_headers, "Expect:");
		_upload_headers = curl_slist_append(_upload_headers, "Content-Type: multipart/form-data");
		
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _common_headers);

		curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_response);

		if (!_access_token.empty()) {
			#define SEP  "\t" 
			string cookie_list =
				"everest.distcomp.org"    /* Hostname */
				SEP "FALSE"      /* Include subdomains */
				SEP "/"          /* Path */
				SEP "FALSE"      /* Secure */
				SEP "0"          /* Expiry in epoch time format. 0 == Session */
				SEP "access_token"        /* Name */
				SEP;       /* Value */

			cookie_list+=_access_token;

			return CURLE_OK == curl_easy_setopt(_curl, CURLOPT_COOKIELIST, cookie_list.c_str());
		}
		else {
			json j;
			j["username"] = _login;
			j["password"] = _pass;
			j["label"] = _label;
			string post = j.dump();

			string link = EVEREST_URL;
			link += "/auth/access_token";

			curl_easy_setopt(_curl, CURLOPT_POST, 1L);
			curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());

			curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, post.c_str());

			_response.clear();
			curl_easy_perform(_curl);
			curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

			if (_code == 200) {
				json responseJSON = json::parse(_response);
				_access_token = responseJSON["access_token"];
				return true;
			}
			return false;
		}
	}

	void everest_engine::cleanup() {
		if (!_curl) return;

		if (!_pass.empty()) {
			string link = EVEREST_URL;
			link += "/api/auth/access_token";

			curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
			curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());

			curl_easy_perform(_curl);
		}

		curl_slist_free_all(_common_headers);
		curl_slist_free_all(_upload_headers);

		curl_easy_cleanup(_curl);
		curl_global_cleanup();

		_curl = NULL;
	}

	bool everest_engine::print_app_description(const char* _name) {
		if (!_curl) return false;
		
		string name = _name;

		string link = EVEREST_URL;
		link += "/api/apps/search?name=";
		link += name;

		curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());

		_response.clear();
		curl_easy_perform(_curl);
		curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

		if (_code == 200) {
			std::cout << _response;
			return true;
		}

		return false;
	}
	
	bool everest_engine::upload(const string& file, string& uri){
		if (!_curl) return false;

		curl_mime* form = NULL;
		curl_mimepart* field = NULL;
	
		string link = EVEREST_URL;
		link += "/api/files/temp";

		string file_name;
		char sep1 = '/', sep2 = '\\';

		size_t i1, i2;
		i1 = file.rfind(sep1, file.length());
		i2 = file.rfind(sep2, file.length());
		size_t i = (i1 > i2 && i1 != string::npos) ? i1 : i2;

		if (i != string::npos)	file_name = file.substr(i + 1, file.length() - i);
		else file_name = file;

		form = curl_mime_init(_curl);

		field = curl_mime_addpart(form);
		curl_mime_name(field, "sendfile");
		curl_mime_filedata(field, file.c_str());

		field = curl_mime_addpart(form);
		curl_mime_name(field, "filename");
		curl_mime_data(field, file_name.c_str(), CURL_ZERO_TERMINATED);

		field = curl_mime_addpart(form);
		curl_mime_name(field, "submit");
		curl_mime_data(field, "send", CURL_ZERO_TERMINATED);
		
		curl_easy_setopt(_curl, CURLOPT_MIMEPOST, form);
		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());

		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _upload_headers);
		
		_response.clear();
        curl_easy_perform(_curl);
		curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

		if (_code == 200) {
			json responseJSON = json::parse(_response);
			uri = responseJSON["uri"];
		}
		
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _common_headers);
		curl_mime_free(form);
		
		return _code == 200;
	}

	bool everest_engine::download(const string& file, const string& uri) {
		if (!_curl) return false;

		string link = EVEREST_URL;
		link += uri;

		FILE* h_file = fopen(file.c_str(), "wb");
		if (!h_file) return false;
		
		curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());

		curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, NULL);
		curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, fwrite);
		curl_easy_setopt(_curl, CURLOPT_WRITEDATA, h_file);

		curl_easy_perform(_curl);
		curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

		curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_response);

		fclose(h_file);

		return _code == 200;
	}

	bool everest_engine::remove(const string& uri) {
		if (!_curl) return false;

		string link = EVEREST_URL;
		link += uri;

		curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());

		curl_easy_perform(_curl);
		curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

		return _code == 200;
	}

	bool everest_engine::submit(everest_task&t) {
		if (!_curl) return false;

		assert(t._eng == this && t.idle());

		string link = EVEREST_URL;
		string post;
		event  ev;

		link += "/api/apps/" + t._app_ID;
		post = t._input.dump();

		curl_easy_setopt(_curl, CURLOPT_POST, 1L);
		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());

		curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, NULL);
		curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, post.c_str());

		_response.clear();
		curl_easy_perform(_curl);
		curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

		if (_code == 200 || _code == 201) {
			json j = json::parse(_response);

			ev._id = j["id"];
			ev._state = j["state"];
			ev._task = &t;
	
			_submitted.push_back(ev);

			t._is_idle = false;
			t._is_done = false;

			return true;
		}

		t._is_idle = true;
		t._is_done = false;

		return false;
	}
	
	bool everest_engine::wait_all() {
		assert(++_recount == 1);

		while (!_submitted.empty()) {
			std::list<event>::iterator it = _submitted.begin();

			while (it != _submitted.end()) {
				event& ev = *it;
				if (!_wait_loop_body(ev)) { assert(--_recount == 0); return false; }
				if (ev._task->idle()) {
					everest_task* tsk = ev._task;
					(*tsk->_tsk_adaptor)(tsk->_actor, tsk);
					tsk->_actor->resume(); 
					it = _submitted.erase(it); 
				}	else it++;
			}
			std::this_thread::sleep_for(DELAY);
		}
		assert(--_recount == 0);
		return true;
	}

	bool everest_engine::wait_for(everest_task& t) {
		assert(++_recount == 1);

		while (!t.idle()) {
			std::list<event>::iterator it = _submitted.begin();

			while (it != _submitted.end()) {
				event& ev = *it;
				if (!_wait_loop_body(ev)) { assert(--_recount == 0); return false; }
				if (ev._task->idle()) { it = _submitted.erase(it); } else it++;
			}
			std::this_thread::sleep_for(DELAY);
		}
		assert(--_recount == 0);
		return true;
	}

	bool everest_engine::_wait_loop_body(event&ev) {
		if (!_curl) return false;

		string link = EVEREST_URL;
		link += "/api/jobs/";
		link += ev._id;
		
		curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);
		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());

		curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, NULL);

		_response.clear();
		curl_easy_perform(_curl);
		curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

		if (_code == 200 || _code == 201) {
			json j = json::parse(_response);

			ev._state = j["state"];

			if (ev._state == "DONE") {
				ev._task->_output = j["result"];

				if (ev._task->_deletable) {
					curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
					curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());
					curl_easy_perform(_curl);
				}

				ev._task->_is_done = true;
				ev._task->_is_idle = true;
			}
			else if (ev._state == "FAILED " || ev._state == "CANCELLED") {
				ev._task->_output = j["info"];

				ev._task->_is_done = false;
				ev._task->_is_idle = true;
			}
			
			return true;
		}
		return false;
	}
}
