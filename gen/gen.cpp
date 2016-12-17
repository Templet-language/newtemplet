/*--------------------------------------------------------------------------*/
/*  Copyright 2016 Sergei Vostokin                                          */
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

#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <locale>

using namespace std;

/*
Syntax:
#pragma templet '~' id ['$'] [ '(' ['-'] id ('!'|'?') {',' ['-'] id ('!'|'?')} ')' ] ['=']

Samples:
#pragma templet ~message
#pragma templet ~message =
#pragma templet ~message$(submessage1?,submessage2!,submessage3!,-submessage4?)

Syntax:
#pragma templet '*' id ['$'] [ '(' ('?'| id ('!'|'?') id) {',' id ('!'|'?') id)} ')' ] ['+']

Samples:
#pragma templet *actor
#pragma templet *actor +
#pragma templet *actor$(?,port?message,port!message) +
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
bool getpragma(string&argstring,int&line);
void closeparse();

void lexinit(string&s);
bool getlex(string&lex);
bool ungetlex();

void error(int line)
{
	cerr << "ERROR: bad #pragma at line " << line << endl;
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
	
	if (getlex(lex) && lex == "$"){ m.serilizable = true; }
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
		ungetlex();
		if (!(getlex(lex) && lex == ")")) error(line);
	}

	if (!m.subm.size()) ungetlex();
	
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

	if (getlex(lex) && lex == "$"){ a.serilizable = true; }
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
		ungetlex();
		if (!(getlex(lex) && lex == ")")) error(line);
	}

	if (!a.ports.size()) ungetlex();

	if (getlex(lex) && lex == "+"){ a.initially_active = true; }
	else{ ungetlex(); a.initially_active = false; }

	if (getlex(lex)) error(line);

	return true;
}

void print_message(ostream& s, message& m)
{
	bool comma = false;

	s << "~" << m.name;
	if (m.serilizable) s << "$";

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

	s << "*" << a.name;
	if (a.serilizable) s << "$";

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

void design(ofstream&outf, list<message>&mlist, list<actor>&alist)
{
	outf << "/*$TET$header*/\n"
		"/*$TET$*/\n";

	outf << endl;

	outf << "using namespace TEMPLET;\n\n"
		"class my_engine{\n"
		"public:\n"
		"\tmy_engine(int argc, char *argv[]);\n"
		"\tvoid run();\n"
		"\tvoid map();\n"
		"};\n";

	outf << endl;

	for (message& m : mlist){
		outf << "#pragma templet "; print_message(outf, m); outf << endl;
		outf << "struct " << m.name << "{\n";

		if (!m.subm.empty()){
			bool first = true;
			outf << "\tenum tag{";
			for (auto& x : m.subm){
				if (first){ outf << "TAG_" << x.name; first = false; }
				else outf << ",TAG_" << x.name;
			}
			outf << "};\n";

			outf << endl;

			for (auto& x : m.subm){
				if (!x.dummy){
					outf << "\tstruct " << x.name << "{\n";
					outf << "/*$TET$" << m.name << "$" << x.name << "*/\n"
						"/*$TET$*/\n";
					outf << "\t} _" << x.name << ";\n\n";
				}
			}

			outf << "\tvoid send(tag);\n";
			outf << "\tbool access(tag);\n";
		}
		else{
			if (m.duplex){
				outf << "\tvoid send();\n";
				outf << "\tbool access();\n";
			}
			else{
				outf << "\tvoid send(actor*);\n";
				outf << "\tbool access();\n";
			}
		}

		if (m.serilizable){
			outf << endl;
			outf << "\tvoid save(saver*s){\n"
				"/*$TET$" << m.name << "$$save*/\n"
				"\t\t//::save(s, &x, sizeof(x));\n"
				"/*$TET$*/\n"
				"\t}\n\n"
				"\tvoid restore(restorer*r)\n"
				"/*$TET$" << m.name << "$$restore*/\n"
				"\t\t//::restore(r, &x, sizeof(x));\n"
				"/*$TET$*/\n"
				"\t}\n";
		}

		if (m.subm.empty()){
			outf << endl;
			outf << "/*$TET$" << m.name << "$$data*/\n"
				"/*$TET$*/\n";
		}

		outf << "};\n\n";
	}

	for (actor& a : alist){
		outf << "#pragma templet "; print_actor(outf, a); outf << endl;
		outf << "struct " << a.name << "{\n";

		outf << "\t" << a.name << "(my_engine&){\n"
			"/*$TET$" << a.name << "$" << a.name << "*/\n"
			"/*$TET$*/\n"
			"\t}\n\n"

			"\tvoid delay(double);\n"
			"\tvoid at(int);\n"
			"\tvoid stop();\n";

		if (a.initially_active){
			outf << endl;
			outf << "\tvoid start(){\n"
				"/*$TET$" << a.name << "$start*/\n"
				"/*$TET$*/\n"
				"\t}\n";
		}

		if (a.response_any){
			for (auto &m : mlist){
				if (m.subm.empty() && !m.duplex){
					outf << endl;
					outf << "\tvoid recv_" << m.name << "(" << m.name << "&m){\n"
						"/*$TET$" << a.name << "$recv_" << m.name << "*/\n"
						"/*$TET$*/\n"
						"\t}\n";
				}
			}
		}

		for (auto &p : a.ports){
			outf << endl;
			outf << "\tvoid " << p.name << "(" << p.message_name << "&m){\n"
				"/*$TET$" << a.name << "$" << p.name << "*/\n"
				"/*$TET$*/\n"
				"\t}\n";
		}

		if (a.serilizable){
			outf << endl;
			outf << "\tvoid save(saver*s){\n"
				"/*$TET$" << a.name << "$$save*/\n"
				"\t\t//::save(s, &x, sizeof(x));\n"
				"/*$TET$*/\n"
				"\t}\n\n"
				"\tvoid restore(restorer*r)\n"
				"/*$TET$" << a.name << "$$restore*/\n"
				"\t\t//::restore(r, &x, sizeof(x));\n"
				"/*$TET$*/\n"
				"\t}\n";
		}

		outf << endl;
		outf << "/*$TET$" << a.name << "$$code&data*/\n"
			"/*$TET$*/\n";

		if (!a.ports.empty()){
			outf << endl;
			for (auto& x : a.ports){
				if (x.type == port::CLIENT)outf << "\t" << x.message_name << " _" << x.name << ";" << endl;
			}
		}

		outf << "};\n\n";
	}

	outf << "/*$TET$footer*/\n"
		"/*$TET$*/\n";
}

int main(int argc, char *argv[])
{
	if (argc<3){
		cout << "Templet skeleton generator. Copyright Sergei Vostokin, 2016" << endl << endl
			<< "Usage: gen <input file with the templet markup> <skeleton output file>" << endl << endl
			<< "The Templet markup syntax:" << endl << endl
			<< "#pragma templet '~' id ['$'] \n\t [ '(' ['-'] id ('!'|'?') {',' ['-'] id ('!'|'?')} ')' ] ['=']" << endl
			<< "#pragma templet '*' id ['$'] \n\t [ '(' ('?'| id ('!'|'?') id) {',' id ('!'|'?') id)} ')' ] ['+']";
		return EXIT_FAILURE;
	}

	if (!openparse(string(argv[1]), string("templet"))){
		cout << "ERROR: could not open file '" << argv[1] << "' for reading" << endl;
		return EXIT_FAILURE;
	}

	list<message> mlist;
	list<actor> alist;

	int line;
	string pragma;

	actor act;
	message msg;

	while (getpragma(pragma,line)){
		cout << "line:" << line << endl;
		cout << pragma << endl;

		lexinit(pragma);
		act.ports.clear();
		msg.subm.clear();

		if (parse_message(line, msg))
			mlist.push_back(msg);
		else if (ungetlex() && parse_actor(line, act))
			alist.push_back(act);
		else
			error(line);
	}

	ofstream outf(argv[2]);

	if (!outf){
		cout << "ERROR: could not open file '" << argv[2] << "' for writing" << endl;
		return EXIT_FAILURE;
	}

	design(outf,mlist,alist);

	closeparse();

	return EXIT_SUCCESS;
}
