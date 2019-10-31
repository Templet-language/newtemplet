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

#include "json.hpp"
#include "curl/curl.h"

#include <cassert>
#include <functional>
#include <string>
#include <list>
#include <chrono>
#include <thread>

using json = nlohmann::json;
using namespace std;

const string EVEREST_URL = "https://everest.distcomp.org";
const std::chrono::seconds delay(1);

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
			init();
		}

		~taskengine() {	cleanup();	}

		bool wait_all();
		bool wait_for(task& t);
		task* wait_some();

		bool submit(task&t);
		bool is_idle(task&t);
		bool is_done(task&t);

		bool get_app_description(const char* _id);

	private:
		inline bool _wait_loop_body(event&ev);
		inline bool init();
		inline bool cleanup();

	private:
		string _login;
		string _pass;
		string _label;
		string _access_token;
		int    _recount;
		
		CURL*  _curl;
		long   _code;
		string _response;

		list<event> _submitted;
	};

	class task {
		friend	class taskengine;
	public:
		task() {} // only for compatibility with preprocessor's design mode

		task(taskengine&e,const char* app_id=0) :
			_eng(&e), _is_idle(true), _is_done(false), _on_ready([]() {}), _app_ID(app_id), _deletable(false) {}
		void set_app_id(const char*id) { _app_ID = id; }
		void set_cleanup() { _deletable = true; }
		
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
		curl_easy_setopt(_curl, CURLOPT_VERBOSE, false);

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

		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());

		curl_easy_setopt(_curl, CURLOPT_POST, 1L);
		curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, post.c_str());

		curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_response);
		_response.clear();

		curl_easy_perform(_curl);

		curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_code);

		if (_code == 200) {
			json responseJSON = json::parse(_response);
			_access_token = responseJSON["access_token"].dump();
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
		string name = _name;
		if (!_curl) return false;

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

	bool taskengine::submit(task&t) {
		assert(t._eng == this && t.is_idle() && _curl);

		string link = EVEREST_URL;
		string post;
		event  ev;

		link += "/api/apps/" + t._app_ID;
		post = t._input.dump();

		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());
		curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, NULL);
		curl_easy_setopt(_curl, CURLOPT_POST, 1L);
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
			std::this_thread::sleep_for(delay);
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
			std::this_thread::sleep_for(delay);
		}
		assert(--_recount == 0);
		return true;
	}

	task* taskengine::wait_some() {
		assert(++_recount == 1);

		while (!_submitted.empty()) {
			std::list<event>::iterator it = _submitted.begin();

			while (it != _submitted.end()) {
				event& ev = *it;
				if (!_wait_loop_body(ev)) { assert(--_recount == 0); return 0; }
				if (ev._task->is_idle()) { ev._task->_on_ready(); it = _submitted.erase(it); return ev._task; }	else it++;
			}
			std::this_thread::sleep_for(delay);
		}
		assert(--_recount == 0);
		return 0;
	}

	bool taskengine::_wait_loop_body(event&ev) {
		assert(_curl);

		string link = EVEREST_URL;
		link += "/api/jobs/";
		link += ev._id;

		curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, NULL);
		curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);

		curl_easy_setopt(_curl, CURLOPT_URL, link.c_str());
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
