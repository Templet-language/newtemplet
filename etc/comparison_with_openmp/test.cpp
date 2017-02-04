#include <omp.h>
#include <stdlib.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

using namespace std;

const int SCALE = 1500;

const int W = SCALE*2;
const int H = SCALE;
const int T = SCALE*2;

double field[H][W];
double field1[H][W];

const int OBS_N = 19;

double obs_seq[OBS_N];
double seq_max, seq_mid, seq_min;

double obs_omp[OBS_N];
double omp_max, omp_mid, omp_min;

double obs_tet[OBS_N];
double tet_max, tet_mid, tet_min;

void shufle()
{
	srand(0);
	for(int i=0;i<H;i++)
	for(int j=0;j<W;j++) field[i][j]=rand();
}

void shufle1()
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
#pragma omp for schedule(dynamic,1)
			for (int i = 1; i<H - 1; i += 2)
				if (i <= t && i>t - 2 * T)	op(i);
		}
		if (t % 2 == 0){
#pragma omp for schedule(dynamic,1)
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
	seq_mid = (seq_min + seq_max) / 2;
	omp_mid = (omp_min + omp_max) / 2;
	tet_mid = (tet_min + tet_max) / 2;
}

struct message;
struct actor;

struct engine{
	volatile int active;
	std::mutex mtx;
	std::condition_variable cv;
	std::queue<message*> ready;
};

struct actor{
	std::mutex mtx;
	void(*recv)(message*, actor*);
};

struct message{
	actor*a;
	bool sending;
};

inline void send(engine*e, message*m, actor*a)
{
	if (m->sending) return;
	m->sending = true;	m->a = a;
	std::unique_lock<std::mutex> lck(e->mtx);
	e->ready.push(m);	e->cv.notify_one();
}

inline bool access(message*m, actor*a)
{
	return m->a == a && !m->sending;
}

void tfunc(engine*e)
{
	message*m; actor*a;

	for (;;){
		{
			std::unique_lock<std::mutex> lck(e->mtx);
			while (e->ready.empty()){
				e->active--;
				if (!e->active){ e->cv.notify_one(); return; }
				e->cv.wait(lck);
				e->active++;
			}
			m = e->ready.front();
			e->ready.pop();
		}
		a = m->a;
		{
			std::unique_lock<std::mutex> lck(a->mtx);
			m->sending = false;
			a->recv(m, a);
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
actor  as[N];
message ms[N - 1];
int  ts[N];

void recv(message* , actor* a)
{
	int id = (int)(a - as);

	if ((id == 0 || access(&ms[id - 1], a)) &&
		(id == N - 1 || access(&ms[id], a)) &&
		(ts[id] <= T)){

		op(id+1);	ts[id]++;

		if (id != 0)	send(&e, &ms[id - 1], &as[id - 1]);
		if (id != N - 1)	send(&e, &ms[id], &as[id + 1]);
	}
}

double par_tet()
{
	for (int i = 0; i < N; i++)
	{
		as[i].recv = recv; ts[i] = 1;
	}
	for (int i = 0; i < N - 1; i++)
	{
		ms[i].a = &as[i]; ms[i].sending = false;
	}
	send(&e, &ms[0], &as[0]);

	return run(&e, omp_get_max_threads());
}

void main()
{
	for (int i = 0; i < OBS_N; i++){
		shufle1();	obs_seq[i] = seq_alg();
		shufle();	obs_omp[i] = par_omp(); cout << (compare() ? "OMP Ok " : "something wrong in OMP ");
		shufle();	obs_tet[i] = par_tet(); cout << (compare() ? "Templet Ok " : "something wrong in Templet ");
		cout << (int)((float)(i+1)/OBS_N*100) << "% done" << endl;
	}
	
	min_max();	

	cout << "\nTest results for H = " << H << "; W = " << W << "; T = " << T << "; OMP max threads  = " << omp_get_max_threads() << "\n";
	
	cout << "\nseq_min  = " << seq_min << " sec; " << "\nseq_mid  = " << seq_mid <<" sec; \nseq_max  = " << seq_max << " sec\n"; 
	cout << "\nomp_min  = " << omp_min << " sec; " << "\nomp_mid  = " << omp_mid << " sec; \nomp_max  = " << omp_max << " sec\n";
	cout << "\ntet_min  = " << tet_min << " sec; " << "\ntet_mid  = " << tet_mid << " sec; \ntet_max  = " << tet_max << " sec\n";
}

/*
public class MyActor extends UntypedActor{
	public int id;
	
	public bool access_ms_id_minus_1 = false;
	public bool access_ms_id = false;

	public ActorRef id_minus_1 = null;
	public ActorRef id_plus_1 = null;

	public void onRecieve(Object m){ // Akka analog of void recv(message* , actor* a)
		if ((Integer)m.intValue() == id - 1) access_ms_id_minus_1 = true;
		if((Integer)m.intValue == id) access_ms_id = true;

		if ((id == 0 || access_ms_id_minus_1) &&
			(id == N - 1 || access_ms_id)) &&
			(ts[id] <= T)){

			op(id + 1);	ts[id]++;

			if (id != 0){id_minus_1.sendOneWay(Integer(id - 1));access_ms_id_minus_1 = false;}
			if (id != N - 1){id_plus_1.sendOneWay(Integer(id));access_ms_id = false;}
		}
	}
}
*/