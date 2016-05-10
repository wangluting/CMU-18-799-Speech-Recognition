#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include "lextree.h"
#include <stack>

using namespace std;

vector<Dict> lexicalTree;            //global variables
vector<Dict> temp_lexicalTree;
vector<wordLookUp> dictionary;
vector<vector<Trellis> > trellis;
stack<char> result;


int main()
{
	// read dictionary and convert it to lexical tree
	cout << "please select what you want to do: 1 means spellchecker without segmentation, 2 means spellchecker with segmentation." << endl;
	int choice;
	cin >> choice;
	if (choice == 1) {
		readDictionary();
		int templateSize = lexicalTree.size();
		trellis.resize(templateSize);
		for (int i = 0; i < templateSize; i++)
			trellis[i].resize(1);
		trellis[0][0].score = 0;        //initial the dummy stata;
		ini_trellis(0); //initial the first line
		//cout << "initialization done" << endl;

		// spell checker with lexical dictionary
		ifstream in("F:\\研究生学习资料\\研一\\Speech Recognition Course\\project4\\typos.txt");
		if (!in)
		{
			cout << "No such file of typos.txt" << endl;
			exit(0);
		}

		ofstream out("spellchecked_typos.txt");
		string s;
		int wordIndex = 0;
		time_t start, stop;
		start = time(NULL);
		while (in >> s)
		{
			//cout << "original word is:" << s << endl;
			levenshtein_lexicalTree(s, wordIndex);
			//cout << dictionary[wordIndex].word << endl;
			out << dictionary[wordIndex].word << " ";
		}
		in.close();
		out.close();

		stop = time(NULL);

		cout << "The total computation time is:" << stop - start << "s" << endl;

	}
	else if (choice == 2) {

		readDictionary();
		int templateSize = lexicalTree.size();
		trellis.resize(templateSize);
		for (int i = 0; i < templateSize; i++)
			trellis[i].resize(1);
		trellis[0][0].score = 0;        //initial the dummy stata;
		//ini_trellis(0);					//initial the first line


		ifstream in("F:\\研究生学习资料\\研一\\Speech Recognition Course\\project4\\unsegmented.txt");
		if (!in)
		{
			cout << "No such file of unsegmented.txt" << endl;
			exit(0);
		}


		char c;
		int mincost = 0;
		int mincost_next = MAXVALUE;
		int mincost_leaf = MAXVALUE;  //(0,1)
		int mincost_leaf_index = 0;  //means the first line cannot look back
		time_t start, stop;
		start = time(NULL);
		int n = 0;  // n is the index of the current char
		temp_lexicalTree = lexicalTree;
		while (in >> c)
		{
			if (n == 0)
			{
				ini_trellis_lookback(0, c, mincost, mincost_next, mincost_leaf, mincost_leaf_index);
				
			}
			else 
			{
				levenshtein_lexicalTree_lookback(n, c, mincost, mincost_next, mincost_leaf, mincost_leaf_index);
			}
			mincost = mincost_next;
			n++;
		}
		in.close();




		result.push(temp_lexicalTree[mincost_leaf_index].c);
		traceBack(mincost_leaf_index, n - 1);

		ofstream out("spellchecked_unsegmented.txt");
		while(!result.empty())
		{
			out << result.top();
			result.pop();
		}
		out.close();
		
		stop = time(NULL);

		cout << "The total computation time is:" << stop - start << "s" << endl;



		
		//calculate the total errors between spell-corrected version of the story and the correct story
		ifstream in3("segmented.txt");
		ifstream in2("spellchecked_unsegmented.txt");
		if (!in3)
		{
			cout << "No such file" << endl;
			exit(0);
		}
		if (!in2)
		{
			cout << "No such file" << endl;
			exit(0);
		}
		string s;
		vector<string> s1;
		vector<string> s2;
		s1.push_back("$");
		s2.push_back("$");

		int numberOfWord = 0;
		while (in3 >> s)
		{
			s1.push_back(s);
		}
		in3.close();
		while (in2 >> s)
		{
			s2.push_back(s);
			numberOfWord++;
		}
		in2.close();

		int insertion = 0;
		int deletion = 0;
		int substution = 0;

		int flag = 1;
		levenshtein_computeAccuracy(s1, s2, flag, insertion, deletion, substution);

		cout << "The total number of word is:" << numberOfWord << endl;
		cout << "The total insertion is:" << insertion << endl;
		cout << "The total deletion is:" << deletion << endl;
		cout << "The total substution is:" << substution << endl;
		cout << "The total errors are:" << insertion + deletion + substution << endl;
		cout << "Accuracy is:" << 1-(insertion + deletion + substution)*1.0 / numberOfWord << endl;
		
	}
	cout << "finished" << endl;
	while (1);
}