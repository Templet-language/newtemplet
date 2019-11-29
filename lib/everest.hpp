/*--------------------------------------------------------------------------*/
/*  Copyright 2019 Sergei Vostokin                                          */
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

using json = nlohmann::json;
using namespace std;

const string EVEREST_URL = "https://everest.distcomp.org";
const std::chrono::seconds DELAY(1);
const bool VERBOSE = false;

namespace TEMPLET {

	class task;
	
	size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
		((string *)userp)->append((char *)contents, size * nmemb);
		return size * nmemb;
	}

	struct event{
		string _state;
		string _id;
		task*  _task;
	};

	class taskengine {
	public:
		taskengine(const char*login, const char*pass, const char* label=0) {
			_login = login; _pass = pass; _curl = NULL; _recount = 0;
			if (label)_label = label; else _label = "templet-session";
			_is_ready = init();
		}

		~taskengine() {	cleanup(); }
		explicit operator bool() { return _is_ready; }

		bool wait_all();
		bool wait_for(task& t);
		bool wait_some();

		bool submit(task&t);
		bool is_idle(task&t);
		bool is_done(task&t);

		bool get_app_description(const char* _id);

		bool upload(const string& file, string& uri);
		bool download(const string& file, const string& uri);
		bool remove(const string& uri);

	private:
		inline bool _wait_loop_body(event&ev);
		inline bool init();
		inline bool cleanup();

	private:
		string _login;
		string _pass;
		string _label;
		string _access_token;
		bool   _is_ready;
		int    _recount;
		
		CURL*  _curl;
		long   _code;
		string _response;

		list<event> _submitted;
	};

	class task {
		friend	class taskengine;
	public:
		task(taskengine&e,const char* app_id=0) :
			_eng(&e), _is_idle(true), _is_done(false), _on_ready([]() {}), _app_ID(app_id), _deletable(false) {}
		task() : _eng(0), _is_idle(true), _is_done(false), _on_ready([]() {}), _app_ID(0), _deletable(false) {} 

		bool set_app_id(const char*id) { if (_is_idle) { _app_ID = id; return true; } return false; }
		bool set_engine(taskengine&e) { if (_is_idle) { _eng = &e; return true; } return false; }
		void set_deletable(bool del) { _deletable = del; }
		
		json& result() { return _result; }
		void  input(json&in) { _input = in; }

		bool submit() { return _eng->submit(*this); }
		void set_on_ready(function<void(void)> callee) { _on_ready = callee; }
		
		bool submit(json& in) { _input = in; return _eng->submit(*this); }

		bool submit(json& in, function<void(void)> on_ready) {
			_input = in; _on_ready = on_ready; return _eng->submit(*this);
		}

		bool submit(function<void(void)> on_ready) {
			_on_ready = on_ready; return _eng->submit(*this);
		}

		bool is_idle() { return _is_idle; }
		bool is_done() { return _is_done; }

	private:
		json   _input;
		json   _result;
		string _app_ID;
		function<void(void)> _on_ready;
		taskengine* _eng;
		bool _is_idle;
		bool _is_done;
		bool _deletable;
	};

	bool taskengine::is_idle(task&t) { return t._is_idle; }
	bool taskengine::is_done(task&t) { return t._is_done; }

	bool taskengine::init() {
		curl_global_init(CURL_GLOBAL_ALL);

		_curl = curl_easy_init();
		if (!_curl) return false;

		curl_easy_setopt(_curl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(_curl, CURLOPT_VERBOSE, VERBOSE);

		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8");
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);

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

		curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_response);

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

	bool taskengine::cleanup() {
		if (!_curl) return false;

		string link = EVEREST_URL;
		link += "/api/auth/access_token";

		curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());

		curl_easy_perform(_curl);
		curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

		curl_easy_cleanup(_curl);
		curl_global_cleanup();

		_curl = NULL;

		return  _code == 200;
	}

	bool taskengine::get_app_description(const char* _name) {
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

	bool taskengine::upload(const string& file, string& uri){
		if (!_curl) return false;

/*-------------------MIMEPOST------------------*/
		struct curl_slist* headers = NULL;
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

		//headers = curl_slist_append(headers, "Expect:");
		headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);
		
		_response.clear();
        curl_easy_perform(_curl);
		curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

		if (_code == 200) {
			json responseJSON = json::parse(_response);
			uri = responseJSON["uri"];
		}

		headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8");
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);
		
		return _code == 200;
		
/*-------------------POST--------------------		
		FILE* h_file = fopen(file.c_str(), "rb");
		if (!h_file) return false;

		struct stat file_info;
		if (fstat(_fileno(h_file), &file_info) != 0) return false;

		string link = EVEREST_URL;
		link += "/api/files/temp";

		curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());
		
		struct curl_slist *headers = NULL;
		//headers = curl_slist_append(headers, "Expect:");
		headers = curl_slist_append(headers, "Content-Type: application/octet-stream");
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(_curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(_curl, CURLOPT_READFUNCTION, &fread);
		curl_easy_setopt(_curl, CURLOPT_READDATA, h_file);
		curl_easy_setopt(_curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);

		_response.clear();
		curl_easy_perform(_curl);
		curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

		headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8");
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(_curl, CURLOPT_UPLOAD, NULL);

		if (_code == 200) {
			json responseJSON = json::parse(_response);
			uri = responseJSON["uri"];
			fclose(h_file);
			return true;
		}
		fclose(h_file);
		return false;
*/
	}

	bool taskengine::download(const string& file, const string& uri) {
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

	bool taskengine::remove(const string& uri) {
		if (!_curl) return false;

		string link = EVEREST_URL;
		link += uri;

		curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());

		curl_easy_perform(_curl);
		curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

		return _code == 200;
	}

	bool taskengine::submit(task&t) {
		if (!_curl) return false;

		assert(t._eng == this && t.is_idle());

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
	
	bool taskengine::wait_all() {
		assert(++_recount == 1);

		while (!_submitted.empty()) {
			std::list<event>::iterator it = _submitted.begin();

			while (it != _submitted.end()) {
				event& ev = *it;
				if (!_wait_loop_body(ev)) { assert(--_recount == 0); return false; }
				if (ev._task->is_idle()) { ev._task->_on_ready(); it = _submitted.erase(it); }	else it++;
			}
			std::this_thread::sleep_for(DELAY);
		}
		assert(--_recount == 0);
		return true;
	}

	bool taskengine::wait_for(task& t) {
		assert(++_recount == 1);

		while (!t.is_idle()) {
			std::list<event>::iterator it = _submitted.begin();

			while (it != _submitted.end()) {
				event& ev = *it;
				if (!_wait_loop_body(ev)) { assert(--_recount == 0); return false; }
				if (ev._task->is_idle()) { ev._task->_on_ready(); it = _submitted.erase(it); }	else it++;
			}
			std::this_thread::sleep_for(DELAY);
		}
		assert(--_recount == 0);
		return true;
	}

	bool taskengine::wait_some() {
		assert(++_recount == 1);

		while (!_submitted.empty()) {
			std::list<event>::iterator it = _submitted.begin();

			while (it != _submitted.end()) {
				event& ev = *it;
				if (!_wait_loop_body(ev)) { 
					assert(--_recount == 0); 
					return false; 
				}
				if (ev._task->is_idle()) {  
					ev._task->_on_ready(); 
					it = _submitted.erase(it); 
					assert(--_recount == 0);
					return true; 
				}	
				else it++;
			}
			std::this_thread::sleep_for(DELAY);
		}
		assert(--_recount == 0);
		return true;
	}

	bool taskengine::_wait_loop_body(event&ev) {
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
				ev._task->_result = j["result"];

				if (ev._task->_deletable) {
					curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
					curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());
					curl_easy_perform(_curl);
				}

				ev._task->_is_done = true;
				ev._task->_is_idle = true;
			}
			else if (ev._state == "FAILED " || ev._state == "CANCELLED") {
				ev._task->_result = j["info"];

				ev._task->_is_done = false;
				ev._task->_is_idle = true;
			}
			
			return true;
		}
		return false;
	}
}
