#include <iostream>
#include <vector>
#include <string>
#ifndef LEXTREE_H
#define LEXTREE_H

#define MAXVALUE 1E9
#define THRESHOLD 15
#define BACKSCORE 0
#define THRESHOLD_WORD 3
using namespace std;

struct Dict {
	char c;
	vector<int> childNode;
};

struct wordLookUp {
	int endindex;
	string word;
};



struct Trellis {
	int score;
	int i;
	int j;
	bool valid;
	int insertion;
	int deletion;
	int substition;
};


using namespace std;

void traceBack(int m, int n);

int min3(int a, int b, int c, int &from);

void search_trellis_nopruning(int index,string s, int i);

bool search_trellis_pruning(int index, string s, int i, int mincost, int &mincost_next, bool &dirty);

bool search_trellis_lookback_pruning(int index, char c, int n, int mincost, int &mincost_next, int &mincost_leaf, bool &dirty, int &mincost_leaf_index);

void search_trellis_lookback_nopruning(int index, char c, int n, int mincost, int &mincost_next, int &mincost_leaf, int &mincost_leaf_index);

void ini_trellis(int index);

void ini_trellis_lookback(int index, char c, int mincost, int &mincost_next, int &mincost_leaf, int &mincost_leaf_index);

void insertChar(int &insertindex, int &insertc, string s);

void readDictionary();

void levenshtein_lexicalTree(string s, int &wordIndex);

void levenshtein_lexicalTree_lookback(int n, char c, int &mincost, int &mincost_next, int &mincost_leaf, int &mincost_leaf_index);

void levenshtein_computeAccuracy(vector<string> s1, vector<string> s2, int flag, int &in, int &de, int &sub);
#endif