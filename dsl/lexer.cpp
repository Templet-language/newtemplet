#include <string>
using namespace std;

static string str;
static string lastlexem;
static bool is_unget;
static int index;
static int len;

enum State {S,A,B,C,D,P,F,E};

State Sta;

void lexinit(string&s)
{
	str = s;
	index = 0;
	is_unget = false;
	lastlexem.clear();
	len = str.length();
}

bool getlex(string&lex)
{
	if (is_unget){
		lex = lastlexem;
		is_unget = false;
		return true;
	}
	
	string lexem = "";
	Sta = State::S;
	char symb;
	while ((Sta != State::F) && (index < len))
	{
		symb = str[index];
		switch (Sta)
		{
		case S:
			if (symb == ' ')
			{
				Sta = State::S;
				index++;
			}
			else
			{
				if (symb == '\\')
				{
					Sta = State::A;
					index++;
				}
				else
				{
					if (symb == '_')
					{
						lexem += symb;
						Sta = State::D;
						index++;
					}
					else
					{
						if (((symb >= 'a') && (symb <= 'z')) || ((symb >= 'A') && (symb <= 'Z')))
						{
							lexem += symb;
							Sta = State::C;
							index++;
						}
						else
						{
							lexem = symb;
							index++;
							Sta = State::F;
						}
					}
				}
			}
			break;
		case A:
			if ((symb == '\r') || (symb == '\t') || (symb == '_'))
			{
				Sta = State::A;
				index++;
			}
			else
				if (symb == '\n')
				{
					Sta = State::S;
					index++;
				}
				else
				{
					lexem = '\\';
					Sta = State::F;
				}
			break;
		case D:
			if (symb == '_') 
			{
				lexem += symb;
				Sta = State::D;
				index++;
			}
			else 
			{
				if (((symb >= 'a') && (symb <= 'z')) || ((symb >= 'A') && (symb <= 'Z')))
				{
					lexem += symb;
					Sta = State::C;
					index++;
				}
				else
				{
					Sta = State::F;
				}
			}
			break;
		case C:
			if (len == index)/////////////
			{
				Sta = State::F;
			}
			else
			{
					if (((symb >= 'a') && (symb <= 'z')) || ((symb >= 'A') && (symb <= 'Z')) || ((symb >= '0') && (symb <= '9')) || (symb=='_'))
					{
						Sta = State::C;
						lexem += symb;
						index++;
					}
					else
					{
						Sta = State::F;
					}
				
			}
			break;
		}
	}
	if ((lexem == ""))
	{
		lastlexem = lexem;
		return false;
	}
	else 
	{
		lex = lexem;
		lastlexem = lexem;
		return true;
	}
}

bool ungetlex()
{
	if (lastlexem!="") 
	{
		is_unget = true;
		return true;
	}
	else{
		is_unget = false;
		return false;
	}
		
}