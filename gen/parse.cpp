#include <iostream>
#include <fstream>
#include <regex>
using namespace std;

ifstream file;
int _line = 0;
string _pragma;

void closeparse();

bool openparse(string&name, string&pragma)
{
	if (file.is_open())
		closeparse();

	file = ifstream(name);

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
		string str = buf;
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
	}
	return false;
}

void closeparse()
{
	file.close();
}