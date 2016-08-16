/*$TET$templet$header*/
/*$TET$*/


struct Result{
    void save(ostream&s){
/*$TET$save$result*/
/*$TET$*/ 
    }
    void rest(istream&s){
/*$TET$restore$result*/
/*$TET$*/
    }
/*$TET$result*/
/*$TET$*/
};

struct Task{
    void save(ostream&s){
/*$TET$save$task*/
/*$TET$*/
    }
    void rest(istream&s){
/*$TET$restore$task*/ 
/*$TET$*/ 
    }
/*$TET$task*/
/*$TET$*/
};

void Proc(Task&t,Result&r){
/*$TET$Proc*/
/*$TET$*/
}

struct Bag{
    Bag(int argc, char* argv[]){
/*$TET$bag$constructor*/
/*$TET$*/
    }
    void run();

    bool isTask(){
/*$TET$bag$isTask*/     
/*$TET$*/
    }
    void put(Result&){
/*$TET$bag$put*/
/*$TET$*/
    }
    void get(Task&t){
/*$TET$bag$get*/
/*$TET$*/
    }

/*$TET$bag*/
/*$TET$*/
};

int main(int argc, char* argv[])
{
    Bag bag(argc,argv);

/*$TET$input*/
/*$TET$*/ 
    bag.run();
/*$TET$output*/
/*$TET$*/
    return 0;
}