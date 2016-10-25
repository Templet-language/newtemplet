/*$TET$header*/
	// TODO: place additional headers and other stuff here
#include <templet.hpp>
	//	is often needed
/*$TET$*/

using namespace TEMPLET;

// состо€ние задачи
struct task{
// сохранение задачи перед отправкой рабочему процессу
	void save(saver*s){
/*$TET$task$save*/
	// TODO: define serialization code here
	// with ::save(s, &data, sizeof(data)); calls
/*$TET$*/
	}
// восстановление состо€ни€ задачи на рабочем процессе
	void restore(restorer*r){
/*$TET$task$restore*/
	// TODO: define deserialization code here
	// with ::restore(r, &data, sizeof(data)); calls
/*$TET$*/
	}
/*$TET$task$data*/
	// TODO: define task data here
/*$TET$*/
};

// результат выполнени€ задачи
struct result{
// сохранение результата перед отправкой управл€ющему процессу
	void save(saver*s){
/*$TET$result$save*/
	// TODO: define serialization code here
	// with ::save(s, &data, sizeof(data)); calls
/*$TET$*/
	}
// восстановление результата на управл€ющем процессе
	void restore(restorer*r){
/*$TET$result$restore*/
	// TODO: define deserialization code here
	// with ::restore(r, &data, sizeof(data)); calls
/*$TET$*/
	}
/*$TET$result$data*/
	// TODO: define result data here
/*$TET$*/
};

// состо€ние и методы управл€ющего процесса
struct bag{
	bag(int argc, char *argv[]){
/*$TET$bag$init*/
	// TODO: place initialization code here
/*$TET$*/
	}
	void run();
	void delay(double);
// метод извлечени€ задачи, если задачи нет -- возвращает false
	bool get(task*t){
/*$TET$bag$get*/
	// TODO: define getting a task from the the bag code (if return false -- no task have got) 
/*$TET$*/	
	}
// метод помещени€ результата вычислени€ задачи
	void put(result*r){
/*$TET$bag$put*/
	// TODO: define putting result to the bag code
/*$TET$*/	
	}
// сохранение состо€ни€, общего дл€ рабочих процессов
	void save(saver*s){
/*$TET$bag$save*/
	// TODO: define serialization code here
	// with ::save(s, &data, sizeof(data)); calls
	// to copy a part of the master process state to all worker processes
/*$TET$*/
	}
// восстановление общего состо€ни€ на рабочих процессах
	void restore(restorer*r){
/*$TET$bag$restore*/
	// TODO: define deserialization code here
	// with ::restore(r, &data, sizeof(data)); calls
	// to copy a part of the master process state to all worker processes
/*$TET$*/
	}
/*$TET$bag$data*/
	// TODO: define bag data here
/*$TET$*/	
};

void delay(double);

// процедура выполнени€ задачи на рабочем процессе
void proc(task*t,result*r)
{
/*$TET$proc$data*/
	// TODO: define task processing code here
/*$TET$*/
}

/*$TET$footer*/
	// TODO: define you program entry point
	// int main(int argc, char *argv[])
	// including
	// 1) creating the bag object
	// 2) input code
	// 3) run with bag.run()
	// 4) output code
	// 5) performance statistics code with ::stat(...)
/*$TET$*/