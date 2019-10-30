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

using json = nlohmann::json;
using namespace std;

const string EVEREST_URL = "https://everest.distcomp.org";

namespace TEMPLET {

	class task;

	size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
		((string *)userp)->append((char *)contents, size * nmemb);
		return size * nmemb;
	}

	struct event{
		task* _task;
	};

	class taskengine {
	public:
		taskengine(const char*login, const char*pass, const char* label=0) {
			_login = login; _pass = pass; _curl = NULL;
			if (label)_label = label; else _label = "templet-session";
			init();
		}

		~taskengine() {
			cleanup();
		}

		void wait_all() {
			assert(++_recount == 1);
			//_wait_loop_body(ev);
			assert(--_recount == 0);
		}

		void wait_for(task& t) {
			assert(++_recount == 1);
			//_wait_loop_body(ev);
			assert(--_recount == 0);
		}

		task* wait_some() {
			assert(++_recount == 1);
			//_wait_loop_body(ev);
			assert(--_recount == 0);
			return 0;
		}

		void submit(task&t);
		bool is_idle(task&t);

		bool get_app_description(const char* _id);

	private:
		inline void _wait_loop_body(event&ev);
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
	};

	class task {
		friend	class taskengine;
	public:
		task() :_eng(0), _is_idle(true), _on_ready([]() {}) {} // only for compatibility with preprocessor's design mode

		task(taskengine&e,const char* app_id=0) :_eng(&e), _is_idle(true), _on_ready([]() {}), _app_ID(app_id) {}
		void set_app_id(const char*id) { _app_ID = id; }
		
		json& out() { return _output; }
		void  in(json&in) { _input = in; }

		void submit() { _eng->submit(*this); }
		void set_on_ready(function<void(void)> callee) { _on_ready = callee; }
		
		void submit(json& in) { _input = in; _eng->submit(*this); }

		void submit(json& in, function<void(void)> on_ready) {
			_input = in; _on_ready = on_ready; _eng->submit(*this);
		}

		void submit(function<void(void)> on_ready) {
			_on_ready = on_ready; _eng->submit(*this);
		}

		bool is_idle() { return _is_idle; }

	private:
		json   _input;
		json   _output;
		string _app_ID;
		function<void(void)> _on_ready;
		taskengine* _eng;
		bool _is_idle;
	};

	bool taskengine::is_idle(task&t) { return t._is_idle; }

	bool taskengine::init() {
		curl_global_init(CURL_GLOBAL_ALL);

		_curl = curl_easy_init();
		if (!_curl) return false;

		curl_easy_setopt(_curl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);

#ifdef USE_CURL_VERBOSE
		curl_easy_setopt(_curl, CURLOPT_VERBOSE, true);
#else
		curl_easy_setopt(_curl, CURLOPT_VERBOSE, false);
#endif

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

	bool taskengine::get_app_description(const char* _id) {
		string id = _id;
		if (!_curl) return false;

		string link = EVEREST_URL;
		link += "/api/apps/search?name=";
		link += id;

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

	void taskengine::submit(task&t) {
		assert(t._eng == this && t.is_idle());

	}

	void taskengine::_wait_loop_body(event&ev) {

	}
}
