#include <string>
#include <vector>
#ifndef LEVENSHTEIN_H
#define LEVENSHTEIN_H

using namespace std;

struct Dict {
	int length;
	bool valid;
	string value;
};

void levenshtein1(std::string s1, std::string s2, int flag); 

void levenshtein2(std::string s1, std::string s2, int flag, Dict dict[], int numOfWords, int &index);

void levenshtein3(std::string s1, std::string s2, int flag, int &in, int &de, int &sub);

void levenshtein4(vector<string> s1, vector<string> s2, int flag, int &in, int &de, int &sub);



#endif
