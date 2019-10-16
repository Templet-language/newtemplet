#pragma once

#include <vector>
#include <queue>
#include <functional>

namespace TEMPLET {

	class task;

	struct event{
		double _time;
		task* _task;
	};

	class cmp { public: bool operator()(const event&t1, const event&t2) { return t1._time > t2._time; } };

	class taskengine {
	public:
		taskengine() {
			init();
		}

		void init() {
			_Tp = 0; _T1 = 0;
			_Pmax = 0;
		}

		void wait_all() {

		}

		void wait_for(task& t) {

		}
		
		void delay(double) {

		}

		double T1() { return 0; }
		double Tp() { return 0; }
		int Pmax() { return 0; }

	private:
		std::priority_queue<event, std::vector<event>, cmp> _calendar;
		double _Tp;
		double _T1;
		int _Pmax;
	};

	class task {
	public:
		task(taskengine&e){}

		virtual void on_start() {}
		virtual void on_ready() {}
		
		void delay(double t) {

		}
		
		double time() {
			return 0;
		}

		void submit() {
			_on_start = [this]() {this->on_start(); };
			_on_ready = [this]() {this->on_ready(); };
		}

		void submit(std::function<void(void)> on_start, std::function<void(void)> on_ready = []() {}) {
			_on_start = on_start;
			_on_ready = on_ready;
		}

	private:
		std::function<void(void)> _on_start;
		std::function<void(void)> _on_ready;
	};

	

}