#include <string>
#include <list>
#include <iostream>
#include <locale>

using namespace std;

/*
#pragma templet ~message
#pragma templet ~message =
#pragma templet ~message@(submessage1?,submessage2!,submessage3!,-submessage4?)

#pragma templet *actor
#pragma templet *actor +
#pragma templet *actor@(?,port?message,port!message) +

#pragma templet '~' id ['@'] [ '(' ['-'] id ('!'|'?') {',' ['-'] id ('!'|'?')} ')' ] ['=']
#pragma templet '*' id ['@'] [ '(' ('?'| id ('!'|'?') id) {',' id ('!'|'?') id)} ')' ] ['+']
*/

struct submessage{
  string name;
  bool dummy;
  enum {CALL,ANSWER} type;
};

struct message{
  string name;
  bool duplex;
  bool serilizable;
  list <submessage> subm;
};

struct port{
  string name;
  string message_name;
  enum {SERVER,CLIENT} type;
};

struct actor{
  string name;
  bool serilizable;
  bool initially_active;
  bool response_any;
  list<port> ports;
};

bool openparse(string&name,string&pragma);
// открыть файл для разбора по имени,
// pragma – обрабатывать только заданные прагмы, например, #pragma omp
bool getpragma(string&argstring,int&line);
// argstring -- аргументы следующей по порядку разбора команды #pragma,
// line – номер строки файла, с которой начинается прагма
void closeparse();
// закрытие файл

void lexinit(string&s);
// инициализация; s – строка символов для разбора
bool getlex(string&lex);
// извлечение следующей лексемы из строки
// return false – если лексем больше нет
bool ungetlex();
// возврат на одну лексему назад
// return false – если возврат невозможен (например, уже выполнен)
void lexclear();
// очистка

void error(int line)
{
	cerr << "error: bag #pragma at line " << line << endl;
	exit(EXIT_FAILURE);
}

bool is_id(string&s)
{
	locale loc;
	return s.length()>1 || (s.length()==1 && isalpha(s[0],loc));
}

bool parse_message(int line, message& m)
{
	string lex;
	if (getlex(lex) && lex == "~"){
		if (getlex(lex) && is_id(lex)){

		}
	}
	else{
		ungetlex();
		return false;
	}
}

bool parse_actor(int line, actor& a)
{
	string lex;
	if (getlex(lex) && lex == "*"){
		if (getlex(lex) && is_id(lex)){

		}
	}
	else{
		ungetlex();
		return false;
	}
}

void print_message(ostream& s, message& m)
{

}

void print_actor(ostream&s,actor&a)
{

}

int main(int argc, char *argv[])
{
	return EXIT_SUCCESS;
}
