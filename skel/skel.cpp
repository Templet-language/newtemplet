// Copyright 2016 Vadim Mishenev, Sergei Vostokin

#include <iostream>
#include "skel.h"

void Analyzator::analyze() {
	infile.open(in, std::ios_base::in);//режим in
	if (!infile) {//открыть файл не удалось
		std::cerr << "Ќе могу открыть\"" << in << "\"дл€ чтени€\n";
		//exit( -1 );
		return;
	}
	State state = startHead;
	char ch; int i = 0;
	std::string buf, buffinder;
	while (infile.get(ch)) {
		//std
		//ищем начало
		if (state == startHead) {
			if (ch == startHeaderText[i]) {
				if (i == startHeaderText.length() - 1) {
					state = endHead;
					i = 0;
				}
				else
					i++;
				continue;
			} else if (i > 0) skeletonfile.seekg((int)infile.tellg() - i);
		}
		//ищем конец  заголовка
        if (state == endHead ) {
		//	buf+=ch;
			if (ch == endHeaderText[i] /*|| (ch == endHeaderText[0] && (i=0))*/) {
				if (i == endHeaderText.length() - 1) {
					state = endBlock;
					buf.clear(); buffinder.clear();
					i = 0;
				} else {
					//buffinder += ch;
					i++;
				}
				continue;
			}	else if (i > 0) {

					key += endHeaderText[0];
					infile.seekg((int)infile.tellg() - i);
				/*	for (int j = 1, ji=0; j < buffinder.length; j++) {
						if (buffinder[j] == endHeaderText[ji]) {

						}
					}*/
				} else {
					key += ch;
			}
			
		}
		//ищем конец  блока
		if (state == endBlock) {
			buf += ch;
			if (ch == endBlockText[i] || (ch == endBlockText[0] && (i = 0) )) {
				if (i == endBlockText.length() - 1) {
					value = buf.substr(0, buf.length() - endBlockText.length());
#ifdef _DEBUG
					std::cout << key << " | "<<value << std::endl; 
#endif

					store.insert(std::pair<std::string, std::string>(key, value));
					key.clear(); value.clear();
					buf.clear();
					state = startHead;
					i = 0;
				}
				else
					i++;
				continue;
			}
		}

		i = 0;
	}
	infile.close();
}
void Analyzator::write() {
	
	skeletonfile.open(skeleton, std::ios_base::in);//режим in
	if (!skeletonfile) {//открыть файл не удалось
		std::cerr << "Ќе могу открыть\"" << skeleton << "\"дл€ чтени€\n";
		//exit( -1 );
		return;
	}

	outfile.open(out, std::ios_base::out);//режим out
	if (!outfile) {//открыть файл не удалось
		std::cerr << "Ќе могу открыть\"" << out << "\"дл€ записи\n";
		//exit( -1 );
		return;
	}

	State state = startHead;
	char ch; int i = 0;
	while (skeletonfile  .get(ch)) {
		//ищем начало
		if (state == startHead ) {	
			if (ch == startHeaderText[i]) {
				if (i == startHeaderText.length() - 1) {
					state = endHead;
					if (!isRealese) outfile << startHeaderText;
					i = 0;
				}
				else 
					i++;
				continue;
				
			} else if (i > 0) {
				outfile << endHeaderText[0];
				skeletonfile.seekg((int)infile.tellg() - i);
			}
			else {
				outfile << ch;
			}
		}
		//ищем конец  заголовка
		if (state == endHead) {
			//	buf+=ch;
			if (ch == endHeaderText[i] /*|| (ch == endHeaderText[0] && (i=0))*/) {
				if (i == endHeaderText.length() - 1) {
					if (!isRealese) outfile << key<< endHeaderText;
					
					state = endBlock;
					i = 0;
				}
				else 
					i++;;
				continue;
			}
			else if (i > 0) {
				key+= endHeaderText[0];
				skeletonfile.seekg((int)infile.tellg() - i);		
			}else {
				key += ch;
			}

		}
		//ищем конец  блока
		if (state == endBlock) {
			if (ch == endBlockText[i] || (ch == endHeaderText[0] && (i = 0))) {
				if (i == endBlockText.length() - 1) {
					std::map<std::string, std::string>::iterator it = store.find(key);
					if (it != store.end()){
#ifdef _DEBUG
						std::cout << key << " | " << it->second;
#endif
						outfile << it->second;
					}
					else {
						std::cout << "Ќеизвестный ключ " << key << "\n";
					}
					if (!isRealese) outfile << endBlockText;
					key.clear();
					state = startHead;
					i = 0;
				}
				else
					i++;
				continue;
			}
		}

		i = 0;
	}
	skeletonfile.close();
	outfile.close();
}
	
	/*простой последовательный поиск
void Analyzator::analyze2() {

	infile.open(in, std::ios_base::in);//режим in
	if (!infile) {//открыть файл не удалось
		std::cerr << "Ќе могу открыть\"" << in << "\"дл€ чтени€\n";
		//exit( -1 );
	}
	State state = startHead;
	char ch; int i = 0;
	std::string buf;
	while (infile.get(ch)) {
		//infile.
		//ищем начало
		if (state == startHead && ch == startHeaderText[i]) {
			if (i == startHeaderText.length() - 1) {
				state = endHead;
				i = 0;
			}
			else
				i++;
			continue;
		}
		//ищем конец  заголовка
		if (state == endHead) {
			buf += ch;
			if (ch == endHeaderText[i] || (ch == endHeaderText[0] && (i = 0))) {
				std::cout << "endHeaderText " << i << ch << endHeaderText[i] << std::endl;
				if (i == endHeaderText.length() - 1) {

					key = buf.substr(0, buf.length() - endHeaderText.length());
					state = endBlock;
					buf.clear();
					i = 0;
				}
				else
					i++;
				continue;
			}
		}
		//ищем конец  блока
		if (state == endBlock) {
			buf += ch;
			if (ch == endBlockText[i] || (ch == endHeaderText[0] && (i = 0))) {
				if (i == endBlockText.length() - 1) {
					value = buf.substr(0, buf.length() - endBlockText.length());
					std::cout << key << " | " << value << std::endl;

					store.insert(std::pair<std::string, std::string>(key, value));
					buf.clear();
					state = startHead;
					i = 0;
				}
				else
					i++;
				continue;
			}
		}

		i = 0;
	}
	infile.close();
}
*/
void Analyzator::error(char *str) {
	std::cerr << str<<std::endl; 
}