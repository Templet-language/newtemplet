// Copyright 2016 Vadim Mishenev, Sergei Vostokin

#include <iostream>
#include <string>
#include "skel.h"

int main(int argc, char* argv[])
{
	setlocale(0, "Russian");
	setlocale(LC_ALL, "");
	bool isRelease = false;
	std::string infile = "infile.cpp", outfile = "outfile.cpp", skeletonfile="skeleton.cpp";
	
	for (int i = 0; i < argc; i++) {
		char *cmd = argv[i];
	
		if (cmd[0] == '-') {
			switch (cmd[1]) {
			case 'O':case 'o':
				outfile = std::string(argv[i + 1]);
				break;
			case 'I':case 'i':
				infile = std::string(argv[i + 1]);
				break;
			case 'S':case 's':
				skeletonfile = std::string(argv[i + 1]);
				break;
			case 'R':case 'r':
				isRelease = true;
			}
		}
	}

#ifdef _DEBUG
	std::cout << infile << std::endl;
	std::cout << outfile << std::endl;
	std::cout << skeletonfile << std::endl;
#endif
	Analyzator analyz(infile, outfile, skeletonfile);
	
	analyz.startHeaderText = "/*$TET$", analyz.endHeaderText = "*/", analyz.endBlockText = "/*$TET$*/";
	analyz.isRealese = isRelease;

	analyz.analyze();

#ifdef _DEBUG
	std::cout << "complete analyze" << std::endl;
#endif

	analyz.write();

#ifdef _DEBUG
	system("pause");
#endif	
	return 0;
}