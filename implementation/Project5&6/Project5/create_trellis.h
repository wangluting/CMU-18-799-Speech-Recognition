#ifndef CREATE_TRELLIS_H
#define CREATE_TRELLIS_H

#include <stack>
#include <vector>
#include "HMMModel.h";
#define MAXVALUE 1E9
#define THRESHOLD 1E9
#define SILENCEPOSIBILITY 0
#define OVERLAPSTATE 3
#define FIRSTPATHSTATE 8
#define SECONDPATHSTATE 5
#define ENDPENALTY 100

using namespace std;

struct Trellis{
	float score;
	int i;
	int j;
	int digit;   // non-emiting state has digit bit to illstrate where it comes from
};

struct TreeMap {
	int digit;
	int index;
	bool valid;
};


void creat_Map();
void trace_back(int m, int n);
void init_trellis(int &m, int n);
void search(vector<vector<float> > mfcc39);



#endif