// Copyright 2016 Vadim Mishenev, Sergei Vostokin

#pragma once

#include <fstream>
#include <string>
#include <map>

class Analyzator {
public:
	std::string startHeaderText, endHeaderText, endBlockText;
	Analyzator(std::string &in, std::string &out, std::string &skeletonnamefile ):in(in), out(out), skeleton(skeletonnamefile) {
		startHeaderText = "/*$TET$", endHeaderText = "*/", endBlockText = "/*$TET$*/";
	}
	bool isRealese;
	void write();
	void analyze();
private:
	void error(char*);
	std::string in, out, skeleton;
	std::fstream infile, outfile, skeletonfile;
	std::map<std::string, std::string> store;
	std::string key, value;
	enum State{ startHead, endHead, endBlock};
};