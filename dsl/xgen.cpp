/*--------------------------------------------------------------------------*/
/*  Copyright 2020 Sergei Vostokin                                          */
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
#include <cstdlib>

using namespace std;

struct port{
  string name;
  string name_type;
  string task_type;
  enum {SERVER,CLIENT,TASK} type;
};

struct actor{
  string name;
  bool initially_active;
  list<port> ports;
};

// from parse.cpp
bool openparse(string&name,string&pragma);
bool getpragma(string&argstring,int&line);
void closeparse();

// from lexer.cpp
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

bool parse_actor(int line, actor& a)
{
	string lex;

	if (getlex(lex) && lex == "!"){ a.initially_active = true; }
	else{ ungetlex(); a.initially_active = false; }

	if (!(getlex(lex) && is_id(lex))) error(line);
	a.name = lex;
	
	if (getlex(lex) && lex == "("){

		port p;

		if (!(getlex(lex) && is_id(lex))) error(line);

		p.name = lex;

		if (getlex(lex) && lex == "?"){ p.type = port::SERVER; }
		else if (ungetlex() && getlex(lex) && lex == "!"){ p.type = port::CLIENT; }
		else error(line);

		if (!(getlex(lex) && is_id(lex))) error(line);

		p.name_type = lex;

		a.ports.push_back(p);

		while (getlex(lex) && lex == ","){

			if (!(getlex(lex) && is_id(lex))) error(line);

			p.name = lex;

			if (getlex(lex) && lex == "?"){ p.type = port::SERVER; }
			else if (ungetlex() && getlex(lex) && lex == "!"){ p.type = port::CLIENT; }
			else if (ungetlex() && getlex(lex) && lex == ":") { p.type = port::TASK;  }
			else error(line);

			if (!(getlex(lex) && is_id(lex))) error(line);

			p.name_type = lex;

			if (p.type == port::TASK) {
				if (getlex(lex) && lex == ".") {
					if (!(getlex(lex) && is_id(lex))) error(line);
					p.task_type = lex;
				}
				else {
					ungetlex();
					p.task_type.clear();
				}
			}

			a.ports.push_back(p);
		}
		ungetlex();
		if (!(getlex(lex) && lex == ")")) error(line);
	}

	if (getlex(lex)) error(line);

	return true;
}

void print_actor(ostream&s,actor&a)
{
	bool comma = false;

	if (a.initially_active) s << "!";
	s << a.name;

	if (a.ports.size()){
		s << "(";

		for(std::list<port>::iterator it = a.ports.begin(); it!= a.ports.end();it++){
			port& x = *it;

			if (comma) s << ",";

			s << x.name;
			if (x.type == port::CLIENT) s << "!";
			else if (x.type == port::SERVER) s << "?";
			else if (x.type == port::TASK)  s << ":"; 
			s << x.name_type;

			if (x.type == port::TASK && !x.task_type.empty()) s << "." << x.task_type;

			comma = true;
		}
		s << ")";
	}
}

void generate(ofstream&outf, list<actor>&alist)
{
	outf << "/*$TET$$header*/\n"
	        "/*$TET$*/\n";

	outf << endl;

	for (std::list<actor>::iterator it = alist.begin(); it != alist.end(); it++) {
		actor& a = *it;

		outf << "#pragma templet "; print_actor(outf, a); outf << endl << endl;

		outf << "struct " << a.name << " :public templet::actor {\n";

		for (std::list<port>::iterator it = a.ports.begin(); it != a.ports.end(); it++) {
			port& p = *it;

			if (p.type == port::CLIENT || p.type == port::SERVER) {
				outf << "\tstatic void on_" << p.name << "_adapter(templet::actor*a, templet::message*m) {\n" <<
					"\t\t((" << a.name << "*)a)->on_" << p.name << "(*(" << p.name_type << "*)m);}\n";
			}
			else if (p.type == port::TASK) {
				if (p.task_type.empty()) {
					outf << "\tstatic void on_" << p.name << "_adapter(templet::actor*a, templet::base_task*t) {\n"
						"\t\t((" << a.name << "*)a)->on_" << p.name << "(*(templet::" << p.name_type << "_task*)t);\n";
				}
				else {
					outf << "\tstatic void on_" << p.name << "_adapter(templet::actor*a, templet::base_task*t) {\n"
						"\t\t((" << a.name << "*)a)->on_" << p.name << "(*(" << p.task_type << "*)t);\n";
				}
			}
		}

		outf << endl;

		if (a.ports.empty()){
			outf << "\t" << a.name << "(templet::engine&e):templet::actor(e) {\n";
	    }
		else {
			outf << "\t" << a.name << "(templet::engine&e):templet::actor(e)";

			for (std::list<port>::iterator it = a.ports.begin(); it != a.ports.end(); it++) {
				port& p = *it;

				if (p.type == port::CLIENT) {
					outf << ",\n";
					outf << "\t\t" << p.name << "(this, &on_" << p.name << "_adapter)";
				}
				else if (p.type == port::TASK) {
					outf << ",\n";
					outf << "\t\t" << p.name << "(te_" << p.name_type << ", this, &on_" << p.name << "_adapter)";
				}

			}

			outf << "\n\t{\n";
		}
		
		outf <<
		"/*$TET$" << a.name << "$" << a.name << "*/\n"
		"/*$TET$*/\n"
		"\t}\n";

		if (a.initially_active) {
			outf << endl;
			outf << "\tvoid on_start(){\n"
				"/*$TET$" << a.name << "$start*/\n"
				"/*$TET$*/\n"
				"\t}\n";
		}

		for (std::list<port>::iterator it = a.ports.begin(); it != a.ports.end(); it++) {
			port& p = *it;
			
			outf << endl;
			
			if (p.type == port::CLIENT || p.type == port::SERVER) {
				
				outf <<
					"\tinline void on_" << p.name << "(" << p.name_type << "&m) {\n"
					"/*$TET$" << a.name << "$" << p.name << "*/\n"
					"/*$TET$*/\n"
					"\t}\n";

			}
			else if (p.type == port::TASK) {
				if (p.task_type.empty()) {
					outf <<
						"\tinline void on_" << p.name << "(templet::" << p.name_type << "_task&t) {\n"
						"/*$TET$" << a.name << "$" << p.name << "*/\n"
						"/*$TET$*/\n"
						"\t}\n";
				}
				else {
					outf <<
						"\tinline void on_" << p.name << "(" << p.task_type << "&t) {\n"
						"/*$TET$" << a.name << "$" << p.name << "*/\n"
						"/*$TET$*/\n"
						"\t}\n";
				}
 			}
		}

		outf << endl;
		
		for (std::list<port>::iterator it = a.ports.begin(); it != a.ports.end(); it++) {
			port& p = *it;

			if (p.type == port::CLIENT ) {
				outf << "\t" << p.name_type <<" " << p.name << ";\n";
			}
			else if (p.type == port::SERVER) {
				outf << "\tvoid " << p.name << "(" << p.name_type << "&m) { m.bind(this, &on_" << p.name << "_adapter); }\n";
			}
			else if (p.type == port::TASK) {
				if(p.task_type.empty())	outf << "\ttemplet::" << p.name_type << "_task " << p.name << ";\n";
				else outf << "\t" << p.task_type << " " << p.name << ";\n";
			}
		}

		outf << endl;
		outf << "/*$TET$" << a.name << "$$footer*/\n"
			    "/*$TET$*/\n";

		outf << "};\n\n";
	}

	outf << "/*$TET$$footer*/\n"
			"/*$TET$*/\n";
}

int main(int argc, char *argv[])
{
	if (argc<3){
		cout<< "Templet code generator. Copyright Sergei Vostokin, 2020" << endl << endl

			<< "Usage: xgen <input: file with the markup> <output: generated C ++ code file>" << endl << endl

			<< "the markup syntax:" << endl << endl

			<< "#pragma templet ['!'] id ['(' field {',' field} ')']" << endl
			<< "field :: = id '!' id |" << endl
			<< "           id '?' id |" << endl
			<< "           id ':' id['.' id]" << endl;

		return EXIT_FAILURE;
	}

	string argv2(argv[1]), tt("templet");
	if (!openparse(argv2,tt)){
		cout << "ERROR: could not open file '" << argv[1] << "' for reading" << endl;
		return EXIT_FAILURE;
	}

	list<actor> alist;

	int line;
	string pragma;
	actor act;

	while (getpragma(pragma,line)){
		cout << "line:" << line << endl;
		cout << pragma << endl;

		lexinit(pragma);
		act.ports.clear();

		if (parse_actor(line, act))
			alist.push_back(act);
		else
			error(line);
	}

	ofstream outf(argv[2]);

	if (!outf){
		cout << "ERROR: could not open file '" << argv[2] << "' for writing" << endl;
		return EXIT_FAILURE;
	}

	generate(outf,alist);

	closeparse();

	return EXIT_SUCCESS;
}
