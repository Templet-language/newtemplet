#include <iostream>
#include <fstream>
//#include <regex>
using namespace std;

ifstream file;
int _line = 0;
string _pragma;

void closeparse();

bool openparse(string&name, string&pragma)
{
	if (file.is_open())
		closeparse();

	file.open(name.c_str());

	if (!file.is_open()) return false;
	
	_pragma = pragma;
	_line = 0;
	
	return true;
}

bool getpragma(string&argstring, int&line)
{
	argstring = ""; line = 0;
	char buf[1000];

	while (file.getline(buf, sizeof(buf)))
	{
		_line++;
		string str(buf);
		size_t pos = 0, last_pos;

		pos = str.find_first_not_of("\t ", pos);
		if (pos == string::npos) continue;

		if(str.find("#", pos) != pos) continue;
		
		pos = str.find_first_not_of("\t ", pos+1);
		if (pos == string::npos) continue;

		if (str.find("pragma", pos) != pos) continue;
		pos += strlen("pragma"); last_pos = pos;
		
		pos = str.find_first_not_of("\t ", pos);
		if (pos == string::npos || pos == last_pos) continue;
		
		if (str.find(_pragma.c_str(), pos) != pos) continue;
		pos += strlen(_pragma.c_str()); last_pos = pos;

		pos = str.find_first_not_of("\t ", pos);
		if (pos == string::npos || pos == last_pos) continue;

		argstring = str.substr(pos);
		line = _line;

		return true;

		/*
		regex rx("[ |\t]*#[ |\t]*pragma[ |\t]+" + _pragma + "[ |\t]+.*");
		if (regex_match(str, rx)) 
		{
			line = _line;

			smatch match;
			regex reg("[ |\t]*#[ |\t]*pragma[ |\t]+" + _pragma + "[ |\t]*(?=\t| )[\t| ^]");
			regex_search(str, match, reg);

			argstring = match.suffix().str();
			while (argstring[argstring.length() - 1] == '\\')
			{
				argstring = argstring.substr(0, argstring.length() - 1);
				file.getline(buf, sizeof(buf));
				argstring += buf;
				_line++;
			}
			return true;
		}
		*/
	}
	return false;
}

void closeparse()
{
	file.close();
}
