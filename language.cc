#include <stdlib.h>
#include <time.h>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <iostream>
#include <algorithm>

using namespace std;

class Language{
	public:
		void searchStrings(int n, int m, string program, string filename);
		void fillSet(string filename, set<string> &words);
		set<string> fillSetNoSpace(set<string> words);
		Language();
};

//constructor, initialize random function for random input
Language::Language(){
	srand(time(NULL));
}

//writes n strings of execution paths from program to filename
void Language::searchStrings(int n, int m, string program, string filename){
	srand(time(NULL));
	int input;
	for(int i=0; i<n; i++){
		input = rand()%m+1;
		ostringstream run;
		run << "./" << program << " " << input;
		ostringstream read;
		read << "./readtracelog.sh " << program << " trace.out >> " << filename;
		system(run.str().c_str());
		system(read.str().c_str());
	}
}

//fills words with string from filename
void Language::fillSet(string filename, set<string> &words){
	ifstream input(filename.c_str());
	for(string line; getline(input, line);){
		words.insert(line + "$");
	}
}

//returns the set words but without spaces in each string
set<string> Language::fillSetNoSpace(set<string> words){
	set<string> wordsNoSpace;
	string temp;
	for(set<string>::iterator it=words.begin(); it!=words.end(); it++){
		temp = *it;
		temp.erase(remove_if(temp.begin(), temp.end(), ::isspace), temp.end());
		temp.erase(remove(temp.begin(), temp.end(), '$'), temp.end());
		wordsNoSpace.insert(temp);
	}
	return wordsNoSpace;
}
