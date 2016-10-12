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
#include <map>
#include <iostream>
#include <fstream>
using namespace std;

enum LTYPE{ FMARK, LMARK, TEXT, FAULTY };

const string lead_sign_beg = "/*$TET$";
const string lead_sign_end = "*/";
const string close_sign = "/*$TET$*/";

const char DELIM[] = " \t";

typedef list<string> Block;

LTYPE linetype(string& line, string& key)
{
	size_t pos;
	if ((pos = line.find(close_sign)) != string::npos){
		// /*tet*/
		size_t ptoken = line.find_first_not_of(DELIM);
		if (ptoken != pos) return TEXT;

		size_t after_close_sign_pos = pos + close_sign.size();
		if (line.find_first_not_of(DELIM, after_close_sign_pos) != string::npos) return FAULTY;

		return LMARK;
	}
	else if ((pos = line.find(lead_sign_beg)) != string::npos){
		// /*tet$key*/
		size_t ptoken = line.find_first_not_of(DELIM);
		if (ptoken != pos) return TEXT;

		size_t key_pos = pos + lead_sign_beg.size();
		size_t after_key_pos = line.find(lead_sign_end, key_pos);

		if (after_key_pos == string::npos) return FAULTY;

		size_t after_mark_pos = after_key_pos + lead_sign_end.size();

		if (line.find_first_not_of(DELIM, after_mark_pos) != string::npos) return FAULTY;

		key = line.substr(key_pos, after_key_pos - key_pos);
		return FMARK;
	}
	else return TEXT;
}

int main(int argc, char* argv[])
{
	bool isRelease = false, helpme = false, place_bottom = false;
	string infile, outfile, skeleton;

	for (int i = 1; i < argc; i++) {
		char *cmd = argv[i];

		if (cmd[0] == '-' || cmd[0] == '/') {
			switch (cmd[1]) {
			case 'O':case 'o':
				outfile = string(argv[i + 1]);
				break;
			case 'I':case 'i':
				infile = string(argv[i + 1]);
				break;
			case 'S':case 's':
				skeleton = string(argv[i + 1]);
				break;
			case 'R':case 'r':
				isRelease = true;
				break;
			case 'H':case 'h':
				helpme = true;
				break;
			case 'B':case 'b':
				place_bottom = true;
				break;
			}
		}
	}

	if (helpme || argc==1){
		cout << "Templet skeleton processor. Copyright Sergei Vostokin, 2016" << endl
			<< "Usage:  skel <options>\n" << endl
			<< " -s -S <skeleton file name>" << endl
			<< " -i -I <input file name>" << endl
			<< " -o -O <output file name> -- assumed to be the same as an input, if not defined" << endl
			<< " -r -R -- use no markup in realease version of the output file" << endl
			<< " -b -B -- place blocks that have no pair in the skeleton into the bottom of output file" << endl
			<< " -h -H -- print this screen" << endl;

	}

	if (outfile.empty()){ outfile = infile; place_bottom = true; }
	if (infile.empty() || skeleton.empty())	{
		cout << "ERROR: bad param(s): input and skeleton files must be defined\n";
		return 1;
	}

	map<string, Block> block_map;

	{
		ifstream file(infile);

		int count = 0;
		bool read_block = false;
		string key;
		Block block;

		if (!file){ cout << "ERROR: could not open input file '" << infile << "'"; return 1; }
		
		string line;
		getline(file, line); count++;
		
		while (file){
			switch (linetype(line, key)){
			case FMARK:
				read_block = true;
				break;
			case LMARK:
				read_block = false;
				block_map[key] = block;
				block.clear();
				break;
			case TEXT:
				if (read_block)block.push_back(line);
				break;
			case FAULTY: cout << "ERROR: a faulty block found in input file '"<< infile<<"' at line #"<<count; return 1;
			}
			getline(file, line); count++;
		}
	}

	{
		string key;
		
		bool read_block = false;
		bool have_substituted_block=false;

		int count = 0;
		ifstream ifile(skeleton);
		ofstream ofile(outfile);

		if (!ifile){ cout << "ERROR: could not open skeleton file '" <<skeleton<< "for reading"; return 1; }
		if (!ofile){ cout << "ERROR: could not open output file '" << outfile << "for writing"; return 1; }

		string line;
		getline(ifile, line); count++;

		while (ifile){
			map<string, Block>::iterator it;
			
			switch (linetype(line, key)){
			case FMARK:
				read_block = true;
				if(!isRelease) ofile << lead_sign_beg << key << lead_sign_end << endl;
				
				it=block_map.find(key);
				if (it != block_map.end()){
					have_substituted_block = true;
					list<string>::const_iterator its;
					for (its = (*it).second.begin(); its != (*it).second.end(); its++){
						ofile << (*its) << endl;
					}
					block_map.erase(it);
				}else
					have_substituted_block = false;
				break;
			case LMARK:
				read_block = false;
				if (!isRelease) ofile << close_sign << endl;
				break;
			case TEXT:
				if (!read_block || (read_block && !have_substituted_block)) ofile << line << endl;
				break;
			case FAULTY: cout << "ERROR: a faulty block found in skeleton file '" << skeleton << "' at line #" << count; return 1;
			}
			getline(ifile, line); count++;
		}

		if (!block_map.empty()){
			cout << "WARNING: not all blocks from the input file '" << infile << "' have a pair in the skeleton file '" << skeleton <<"'" << endl;
			if (place_bottom){
				map<string, Block>::const_iterator it;
				for (it = block_map.begin(); it != block_map.end(); it++){
					ofile << lead_sign_beg << (*it).first << lead_sign_end << endl;
					list<string>::const_iterator its;
					for (its = (*it).second.begin(); its != (*it).second.end(); its++)
						cout << (*its) << endl;
					ofile << close_sign << endl;
				}
			}
		}
	}

	return 0;
}