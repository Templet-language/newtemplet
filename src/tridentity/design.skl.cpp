/*$TET$$header*/
	// TODO: place additional headers and other stuff here
	// #include <templet.hpp> often needed
/*$TET$*/

using namespace TEMPLET;

class my_engine{
public:
	my_engine(int argc, char *argv[]);
	void run();
	void map();
};

class value_message{
public:
	bool access();
	void send();
/*$TET$value_message$$data*/
	// TODO: define message data here
/*$TET$*/
	void save(saver*s){
/*$TET$value_message$$save*/
	// TODO: define serialization code here
	// with ::save(data, &data, sizeof(data)); calls
/*$TET$*/
	}			
	void restore(restorer*r){
/*$TET$value_message$$restore*/
	// TODO: define deserialization code here
	// with ::restore(data, &data, sizeof(data)); calls
/*$TET$*/
	}
};

class master{
public:
	master(my_engine&);

	value_message& sin2_port();
	value_message& cos2_port();
	
	void delay(double);
	void at(int);
	void stop();

	void start(){
/*$TET$master$start*/
	// TODO: define startup operations here
	// use *.send(); messages' methods 
/*$TET$*/
	}

	void sin2_func(value_message&m){
/*$TET$master$sin2_func*/
	// TODO: define message handling operations here
/*$TET$*/
	}

	void cos2_func(value_message&m){
/*$TET$master$cos2_func*/
	// TODO: define message handling operations here
/*$TET$*/
	}

	void save(saver*s){
/*$TET$master$$save*/
	// TODO: define serialization code here
	// with ::save(data, &data, sizeof(data)); calls
/*$TET$*/
	}
	void restore(restorer*r){
/*$TET$master$$restore*/
	// TODO: define deserialization code here
	// with ::restore(data, &data, sizeof(data)); calls
/*$TET$*/
	}

	value_message _cos2;
	value_message _sin2;
/*$TET$master$$data*/
	// TODO: define actor data here
/*$TET$*/
};

class worker{
public:
	worker(my_engine&);

	void master_port(value_message&);

	void master_func(value_message&m){
/*$TET$worker$master_func*/
	// TODO: define message handling operations here
/*$TET$*/
	}

	void at(int);
	void delay(double);
};

/*$TET$templet$footer*/
	// TODO: define you program entry point
	// int main(int argc, char *argv[])
	// including
	// 1) creating execution engine object
	// 2) creating actor's objects
	// 3) actors interconnecting 
	// 4) actors' placement with *.at() and engine.map()
	// 5) input code
	// 6) run with engine.run()
	// 7) output code
	// 8) performance statistics code with ::stat(...)
/*$TET$*/