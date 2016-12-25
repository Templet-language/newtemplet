#include <omp.h>
#include <stdlib.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

using namespace std;

const int SCALE = 800;

const int W = SCALE;
const int H = SCALE;
const int T = SCALE;

double field[H][W];
double field1[H][W];

const int OBS_N = 19;

double obs_seq[OBS_N];
double seq_max, seq_min;

double obs_omp[OBS_N];
double omp_max, omp_min;

double obs_tet[OBS_N];
double tet_max, tet_min;

double NET;

void shufle()
{
	srand(0);
	for(int i=0;i<H;i++)
	for(int j=0;j<W;j++) field[i][j]=rand();
}

void shufle_seq()
{
	srand(0);
	for(int i=0;i<H;i++)
	for(int j=0;j<W;j++) field1[i][j]=rand();
}

void op(int i)
{
	for (int j=1; j<W-1; j++)
		field[i][j]=(field[i][j-1]+field[i][j+1]+
			field[i-1][j]+field[i+1][j])*0.25;
}

void op1(int i)
{
	for (int j = 1; j<W - 1; j++)
		field1[i][j] = (field1[i][j - 1] + field1[i][j + 1] +
		field1[i - 1][j] + field1[i + 1][j])*0.25;
}

double seq_alg()
{
	double time=omp_get_wtime();

	for (int t=1;t<=T;t++)
	for (int i=1;i<H-1;i++) op1(i);
	
	return omp_get_wtime() - time;
}

double par_omp()
{
	double time = omp_get_wtime();

#pragma omp parallel shared(H,T)
{
	for (int t = 1; t <= (2 * T - 1) + (H - 3); t++){

		if (t % 2 == 1){
#pragma omp for schedule(dynamic)
			for (int i = 1; i<H - 1; i += 2)
				if (i <= t && i>t - 2 * T)	op(i);
		}
		if (t % 2 == 0){
#pragma omp for schedule(dynamic)
			for (int i = 2; i<H - 1; i += 2)
				if (i <= t && i>t - 2 * T)	op(i);
		}
	}
}
	return omp_get_wtime() - time;
}

bool compare()
{
	for(int i=0; i<H; i++)
	for(int j=0; j<W; j++) if(field1[i][j]!=field[i][j])return false;
	return true;
}

void min_max()
{
	seq_max = seq_min = obs_seq[0];
	omp_max = omp_min = obs_omp[0];
	tet_max = tet_min = obs_tet[0];

	for (int i = 1; i < OBS_N; i++){
		seq_max = obs_seq[i] > seq_max ? obs_seq[i] : seq_max;
		seq_min = obs_seq[i] < seq_min ? obs_seq[i] : seq_min;

		omp_max = obs_omp[i] > omp_max ? obs_omp[i] : omp_max;
		omp_min = obs_omp[i] < omp_min ? obs_omp[i] : omp_min;

		tet_max = obs_tet[i] > tet_max ? obs_tet[i] : tet_max;
		tet_min = obs_tet[i] < tet_min ? obs_tet[i] : tet_min;
	}
}

void calc_NET()
{
	int max_t = omp_get_max_threads();
	
	double par_time = omp_get_wtime();
#pragma omp parallel
	{
		for (int t = 1; t <= T; t++) op(1 + omp_get_thread_num() * 2);
	}
	par_time = omp_get_wtime() - par_time;
	
	double seq_time = omp_get_wtime();
	for (int t = 0; t < max_t; t++)
		for (int t = 1; t <= T; t++) op(1 + omp_get_thread_num() * 2);
	seq_time = omp_get_wtime() - seq_time;

	NET = seq_time / par_time;
}

struct chan;
struct proc;

struct engine{
	volatile int active;
	std::mutex mtx;
	std::condition_variable cv;
	std::queue<chan*> ready;
};

struct proc{
	std::mutex mtx;
	void(*recv)(chan*, proc*);
};

struct chan{
	proc*p;
	bool sending;
};

inline void send(engine*e, chan*c, proc*p)
{
	if (c->sending) return;
	c->sending = true;	c->p = p;
	std::unique_lock<std::mutex> lck(e->mtx);
	e->ready.push(c);	e->cv.notify_one();
}

inline bool access(chan*c, proc*p)
{
	return c->p == p && !c->sending;
}

void tfunc(engine*e)
{
	chan*c; proc*p;

	for (;;){
		{
			std::unique_lock<std::mutex> lck(e->mtx);
			while (e->ready.empty()){
				e->active--;
				if (!e->active){ e->cv.notify_one(); return; }
				e->cv.wait(lck);
				e->active++;
			}
			c = e->ready.front();
			e->ready.pop();
		}
		p = c->p;
		{
			std::unique_lock<std::mutex> lck(p->mtx);
			c->sending = false;
			p->recv(c, p);
		}
	}
}

double run(engine*e, int n = 1)
{
	std::vector<std::thread> threads(n);
	e->active = n;
	for (int i = 0; i<n; i++) threads[i] = std::thread(tfunc, e);
	double start = omp_get_wtime();
	for (auto& th : threads) th.join();
	return omp_get_wtime() - start;
}

const int N = H - 2;

engine e;
proc ps[N];
chan cs[N - 1];
int  ts[N];

void recv(chan* c, proc* p)
{
	int pid = (int)(p - ps);

	if ((pid == 0 || access(&cs[pid - 1], p)) &&
		(pid == N - 1 || access(&cs[pid], p)) &&
		(ts[pid] <= T)){

		op(pid+1);	ts[pid]++;

		if (pid != 0)	send(&e, &cs[pid - 1], &ps[pid - 1]);
		if (pid != N - 1)	send(&e, &cs[pid], &ps[pid + 1]);
	}
}

double par_tet()
{
	for (int i = 0; i < N; i++)
	{
		ps[i].recv = recv; ts[i] = 1;
	}
	for (int i = 0; i < N - 1; i++)
	{
		cs[i].p = &ps[i]; cs[i].sending = false;
	}
	send(&e, &cs[0], &ps[0]);

	return run(&e, omp_get_max_threads());
}

void main()
{
	for (int i = 0; i < OBS_N; i++){
		shufle_seq();	obs_seq[i] = seq_alg();
		shufle();	obs_omp[i] = par_omp(); cout << (compare() ? "OMP Ok " : "something wrong in OMP ");
		shufle();	obs_tet[i] = par_tet(); cout << (compare() ? "Templet Ok " : "something wrong in Templet ");
	
		cout << (int)((float)(i+1)/OBS_N*100) << "% done" << endl;
	}
	
	min_max();	calc_NET();

	cout << "\nTest results for H = " << H << "; W = " << W << "; T = " << T << "; OMP max threads  = " << omp_get_max_threads() << "\n";
	
	cout << "\nseq_min  = " << seq_min << " sec; " << "\nseq_max  = " << seq_max <<" sec; \ndiff_seq = " << seq_max-seq_min << " sec\n"; 
	cout << "\nomp_min  = " << omp_min << " sec; " << "\nomp_max  = " << omp_max << " sec; \ndiff_omp = " << omp_max - omp_min << " sec\n";
	cout << "\ntet_min  = " << tet_min << " sec; " << "\ntet_max  = " << tet_max << " sec; \ndiff_tet = " << tet_max - tet_min << " sec\n";

	cout << "\nOMP min based speedup = " << seq_min / omp_max << "\n";
	cout << "OMP max based speedup = " << seq_max / omp_min << "\n";

	cout << "\nTET min based speedup = " << seq_min / tet_max << "\n";
	cout << "TET max based speedup = " << seq_max / tet_min << "\n";

	double alfa = ((double)(2 * T - 1) + (H - 3)) / (T * (H - 2));
	double est_speedup_NET = 1 / ((1-alfa)/NET+alfa);
	double est_speedup_OMP = 1 / ((1 - alfa) / omp_get_max_threads() + alfa);

	cout << "\ntheoretical speedup for the problem = " << 1 / alfa << "\n";
	cout << "number of efficient threads (NET)  = "<< NET << "\n";
	cout << "Amdahl's speedup estimation based on NET = " << est_speedup_NET << "\n";
	cout << "Amdahl's speedup estimation based on OMP max threads = " << est_speedup_OMP << "\n";
	
	cout << "\npresicion = " << omp_get_wtick() << " sec";

}

