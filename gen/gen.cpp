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
	cerr << "error: bad #pragma at line " << line << endl;
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
	
	if (!(getlex(lex) && lex == "~")){ ungetlex(); return false; }

	if (!(getlex(lex) && is_id(lex))) error(line);

	m.name = lex;
	
	if (getlex(lex) && lex == "@"){ m.serilizable = true; }
	else{ ungetlex(); m.serilizable = false; }
	
	if (getlex(lex) && lex == "("){

		submessage sm;

		if (getlex(lex) && lex == "-"){ sm.dummy = true; }
		else{ ungetlex(); sm.dummy = false; }
		
		if (!(getlex(lex) && is_id(lex))) error(line);

		sm.name = lex;

		if (getlex(lex) && lex == "?"){ sm.type = submessage::ANSWER; }
		else if (ungetlex() && getlex(lex) && lex == "!"){ sm.type = submessage::CALL; }
		else error(line);

		m.subm.push_back(sm);

		while (getlex(lex) && lex == ","){

			if (getlex(lex) && lex == "-"){ sm.dummy = true; }
			else{ ungetlex(); sm.dummy = false; }

			if (!(getlex(lex) && is_id(lex))) error(line);

			sm.name = lex;

			if (getlex(lex) && lex == "?"){ sm.type = submessage::ANSWER; }
			else if (ungetlex() && getlex(lex) && lex == "!"){ sm.type = submessage::CALL; }
			else error(line);

			m.subm.push_back(sm);
		}
	}
	ungetlex();

	if (!(getlex(lex) && lex == ")")) error(line);

	if (getlex(lex) && lex == "="){ m.duplex = true; }
	else{ ungetlex(); m.duplex = false; }

	if (m.subm.size()) m.duplex = true;

	if (getlex(lex)) error(line);

	return true;
}

bool parse_actor(int line, actor& a)
{
	string lex;

	if (!(getlex(lex) && lex == "*")){ ungetlex(); return false; }

	if (!(getlex(lex) && is_id(lex))) error(line);

	a.name = lex;

	if (getlex(lex) && lex == "@"){ a.serilizable = true; }
	else{ ungetlex(); a.serilizable = false; }

	if (getlex(lex) && lex == "("){

		port p;

		a.response_any = false;

		if (getlex(lex) && lex == "?"){
			a.response_any = true;
		}
		else{
			ungetlex();

			if (!(getlex(lex) && is_id(lex))) error(line);

			p.name = lex;

			if (getlex(lex) && lex == "?"){ p.type = port::SERVER; }
			else if (ungetlex() && getlex(lex) && lex == "!"){ p.type = port::CLIENT; }
			else error(line);

			if (!(getlex(lex) && is_id(lex))) error(line);

			p.message_name = lex;

			a.ports.push_back(p);
		}

		while (getlex(lex) && lex == ","){

			if (!(getlex(lex) && is_id(lex))) error(line);

			p.name = lex;

			if (getlex(lex) && lex == "?"){ p.type = port::SERVER; }
			else if (ungetlex() && getlex(lex) && lex == "!"){ p.type = port::CLIENT; }
			else error(line);

			if (!(getlex(lex) && is_id(lex))) error(line);

			p.message_name = lex;

			a.ports.push_back(p);
		}
	}
	ungetlex();

	if (!(getlex(lex) && lex == ")")) error(line);

	if (getlex(lex) && lex == "+"){ a.initially_active = true; }
	else{ ungetlex(); a.initially_active = false; }

	if (getlex(lex)) error(line);

	return true;
}

void print_message(ostream& s, message& m)
{
	bool comma = false;

	s << "~" << m.name;
	if (m.serilizable) s << "@";

	if (m.subm.size()){
		s << "(";
		for (auto x : m.subm){
			if (comma) s << ",";

			if (x.dummy) s << "-";
			s << x.name;
			if (x.type == submessage::ANSWER) s << "!";
			else if (x.type == submessage::CALL) s << "?";
			
			comma = true;
		}
		s << ")";
	}

	if (m.duplex && m.subm.size() == 0) s << "=";
}

void print_actor(ostream&s,actor&a)
{
	bool comma = false;

	s << "~" << a.name;
	if (a.serilizable) s << "@";

	if (a.ports.size()){
		s << "(";

		if (a.response_any){ s << "?"; comma = true; }

		for (auto x : a.ports){
			if (comma) s << ",";

			s << x.name;
			if (x.type == port::CLIENT) s << "!";
			else if (x.type == port::SERVER) s << "?";
			s << x.message_name;

			comma = true;
		}
		s << ")";
	}

	if (a.initially_active) s << "+";
}

int main(int argc, char *argv[])
{
	return EXIT_SUCCESS;
}
