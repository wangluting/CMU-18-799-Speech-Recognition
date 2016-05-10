#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
#include "lextree.h"

using namespace std;

extern vector<Dict> lexicalTree;
extern vector<Dict> temp_lexicalTree;
extern vector<wordLookUp> dictionary;
extern vector<vector<Trellis> > trellis;
extern stack<char> result;


int counter = 0;

void absolutePathcost(int &value, bool &valid)
{
	if (value > THRESHOLD_WORD)
		valid = 0;
}

void beamSearch(int &value, bool &valid, int T)
{
	if (value > T + THRESHOLD_WORD)
		valid = 0;
}

void traceBack(int m, int n)
{
	if (trellis[m][n].j == 0 && trellis[m][n].i == -1)
	//if(m == 0 && n == 0)
		return;
	if (trellis[m][n].j == 0 )
		result.push(' ');
	else
	{
		result.push(temp_lexicalTree[trellis[m][n].j].c);
	}
	
		traceBack(trellis[m][n].j, trellis[m][n].i);
	//cout << "(" << trellis[m][n].i << "," << trellis[m][n].j << ")->";


}

int min2(int a, int b, int &from)
{
	int  minvalue = a;
	if (minvalue > b)
	{
		minvalue = b;
		from = 2;
	}

	return minvalue;
}

int min3(int a, int b, int c, int &from)
{
	int  minvalue = a;
	if (minvalue > b)
	{
		minvalue = b;
		from = 2;
	}
	if (minvalue > c)
	{
		minvalue = c;
		from = 3;
	}

	return minvalue;
}

void search_trellis_nopruning(int index, string s, int i)
{

	if (lexicalTree[index].childNode[0] != 0)  //not the leaf node
	{
		vector<int> tempv = lexicalTree[index].childNode;
		for (int j = 0; j != tempv.size(); j++)
		{
			//int index = tempv[j];
			int cost = 1;
			int value1 = trellis[index][i].score;
			int value2 = trellis[index][i - 1].score;
			int value3 = trellis[tempv[j]][i - 1].score;
			
			if (s[i] == lexicalTree[tempv[j]].c)
				cost = 0;
			int from = 1;
			int minvalue = min3(value1, value2, value3, from);
			trellis[tempv[j]][i].score = minvalue + cost;

			search_trellis_nopruning(tempv[j], s, i);
			
		}
	}

}

bool search_trellis_pruning(int index, string s, int i, int mincost, int &mincost_next, bool &dirty)
{
	if (temp_lexicalTree[index].childNode[0] == 0)  //it is the leaf node
	{
		
		if (dirty == true)    // has valid transition and no need to prune
		{
			dirty = false;   //reset dirty bit
			return false;    // don't prune
		}
		else {
			//printf("%d is dead.\n", index);
			return true;
		}
			
	}
	
	if (temp_lexicalTree[index].childNode[0] != 0)  //not the leaf node
	{
		vector<int> & tempv = temp_lexicalTree[index].childNode;
		for (int j = 0; j < tempv.size(); j++)
		{
			int cost = 1;
			int value1 = trellis[index][i].score;
			int value2 = trellis[index][i - 1].score;
			int value3 = trellis[tempv[j]][i - 1].score;
			int temp_cost = THRESHOLD + mincost;

			if (value2 > temp_cost) value2 = MAXVALUE;
			if (value3 > temp_cost) value3 = MAXVALUE;
			if (value1 > temp_cost+1) value1 = MAXVALUE;

			if (s[i] == temp_lexicalTree[tempv[j]].c)
				cost = 0;
			int from = 1;
			int minvalue = min3((value1 + 1), (value2 + cost), (value3 + 1), from);
			trellis[tempv[j]][i].score = minvalue;

			int temp_score = trellis[tempv[j]][i].score;
			if (mincost_next >= temp_score)
			{
				mincost_next = temp_score;
			}

			if (value2 != MAXVALUE || value3 != MAXVALUE || value1 != MAXVALUE)
			{
				dirty = true;
			}

			int flag = false;
			flag = search_trellis_pruning(tempv[j], s, i, mincost, mincost_next, dirty);

			if (tempv.size() == 1)
			{
				return flag;
			}
			else if (flag == true)
			{
				//cout << "erasing... " << endl;
				vector<int>::iterator it = tempv.begin() + j;
				tempv.erase(it);
				j--;
			}

		}
	}
	return false;
}

bool search_trellis_lookback_pruning(int index, char c, int n, int mincost, int &mincost_next, int &mincost_leaf, bool &dirty, int &mincost_leaf_index)
{
	
	if (temp_lexicalTree[index].childNode[0] == 0)  //it is the leaf node
	{	
		if (dirty == true)    // has valid transition and no need to prune
		{
			dirty = false;   //reset dirty bit
			return false;    // don't prune
		}
		else
		{
			/*for (int i = 0; i < dictionary.size(); i++)
			{
				if (dictionary[i].endindex == index)
				{
					cout <<"at "<<c<< " erasing:" << dictionary[i].word << endl;
					break;
				}
			}*/
			return true;
		}
	}

	if (temp_lexicalTree[index].childNode[0] != 0)  //not the leaf node
	{
		vector<int> & tempv = temp_lexicalTree[index].childNode;
		for (int j = 0; j < tempv.size(); j++)
		{
			int cost = 1;
			int value1 = trellis[index][n].score;
			int value2 = trellis[index][n-1].score;
			int value3 = trellis[tempv[j]][n-1].score;
			int temp_cost = THRESHOLD + mincost;
			if (value1 > temp_cost+1) value1 = MAXVALUE;
			if (value2 > temp_cost) value2 = MAXVALUE;
			if (value3 > temp_cost) value3 = MAXVALUE;

			if (c == temp_lexicalTree[tempv[j]].c)
				cost = 0;

			int from = 1; // from=1: from value1, from=2:from value2, from=3:from value 3
			int minvalue;
			if (index == 0)  //if it is the son of *
			{
				minvalue = min2((value1 + cost), (value3 + 1), from);
			}
			else
			{
				minvalue = min3((value1 + 1), (value2 + cost), (value3 + 1), from);
			}
			trellis[tempv[j]][n].score = minvalue;
			switch (from){
			case 1:
				trellis[tempv[j]][n].j = index;
				trellis[tempv[j]][n].i = n;
				break;
			case 2:
				trellis[tempv[j]][n].j = index;
				trellis[tempv[j]][n].i = n-1;
				break;
			case 3:
				trellis[tempv[j]][n].j = tempv[j];
				trellis[tempv[j]][n].i = n-1;
				break;
			}


			int temp_score = trellis[tempv[j]][n].score;
			if (temp_lexicalTree[tempv[j]].childNode[0] == 0)
			{
				if (temp_score <= mincost_leaf)
				{
					mincost_leaf = temp_score;
					mincost_leaf_index = tempv[j];

				}
				
			}
			if (mincost_next >= temp_score)
			{
				mincost_next = temp_score;

			}


			if (value2 != MAXVALUE || value3 != MAXVALUE || value1 != MAXVALUE)
			{
				dirty = true;
			}

			int flag = false;
			flag = search_trellis_lookback_pruning(tempv[j], c, n, mincost, mincost_next, mincost_leaf, dirty, mincost_leaf_index);

			if (tempv.size() == 1)
			{
				return flag;
			}
			else if (flag == true)
			{
				//cout << "erasing... " << endl;
				vector<int>::iterator it = tempv.begin() + j;
				tempv.erase(it);
				j--;
			}
			

		}

	}
	return false;
}

void search_trellis_lookback_nopruning(int index, char c, int n, int mincost, int &mincost_next, int &mincost_leaf, int &mincost_leaf_index)
{

	if (temp_lexicalTree[index].childNode[0] != 0)  //not the leaf node
	{
		vector<int> & tempv = temp_lexicalTree[index].childNode;
		for (int j = 0; j < tempv.size(); j++)
		{
			int cost = 1;
			int value1 = trellis[index][n].score;
			int value2 = trellis[index][n-1].score;
			int value3 = trellis[tempv[j]][n-1].score;
			int temp_cost = THRESHOLD + mincost;
			if (value1 > temp_cost + 1) value1 = MAXVALUE;
			if (value2 > temp_cost) value2 = MAXVALUE;
			if (value3 > temp_cost) value3 = MAXVALUE;

			if (c == temp_lexicalTree[tempv[j]].c)
				cost = 0;

			int from = 1; // from=1: from value1, from=2:from value2, from=3:from value 3
			int minvalue;
			if (index == 0)  //if it is the son of the dummy state
			{
				minvalue = min2((value1 + cost), (value3 + 1), from);
			}
			else
			{
				minvalue = min3((value1 + 1), (value2 + cost), (value3 + 1), from);
			}
			trellis[tempv[j]][n].score = minvalue;
			switch (from) 
			{
			case 1:
				trellis[tempv[j]][n].j = index;
				trellis[tempv[j]][n].i = n;
				break;
			case 2:
				trellis[tempv[j]][n].j = index;
				trellis[tempv[j]][n].i = n-1;          
				break;
			case 3:
				trellis[tempv[j]][n].j = tempv[j];
				trellis[tempv[j]][n].i = n-1;
				break;
			}


			int temp_score = trellis[tempv[j]][n].score;
			if (temp_lexicalTree[tempv[j]].childNode[0] == 0)
			{
				if (temp_score <= mincost_leaf)
				{
					mincost_leaf = temp_score;
					mincost_leaf_index = tempv[j];

				}

			}

			if (mincost_next >= temp_score)
			{
				mincost_next = temp_score;

			}

			search_trellis_lookback_nopruning(tempv[j], c, n, mincost, mincost_next, mincost_leaf, mincost_leaf_index);

		}

	}
	return ;
}


void ini_trellis(int index)
{


	if (lexicalTree[index].childNode[0] != 0) // not the leaf node
	{
		vector<int> tempv = lexicalTree[index].childNode;
		for (int j = 0; j != tempv.size(); j++)
		{
			//int value = trellis[index][0].score;
			trellis[tempv[j]][0].score = trellis[index][0].score + 1;
			trellis[tempv[j]][0].j = index;
			trellis[tempv[j]][0].i = 0;
			ini_trellis(tempv[j]);
		}

	}
	return;
}

void ini_trellis_lookback(int index, char c, int mincost, int &mincost_next, int &mincost_leaf, int &mincost_leaf_index)
{
	if (lexicalTree[index].childNode[0] != 0) // not the leaf node
	{
		vector<int> tempv = lexicalTree[index].childNode;
		for (int j = 0; j != tempv.size(); j++)
		{
			if (index == 0)  //the son of *
			{
				int cost = 1;
				if (c == temp_lexicalTree[tempv[j]].c)
					cost = 0;
				trellis[tempv[j]][0].score = trellis[index][0].score + cost;
			}
			else
			{
				trellis[tempv[j]][0].score = trellis[index][0].score + 1;
			}
			trellis[tempv[j]][0].j = index;
			trellis[tempv[j]][0].i = -1;
			
			int temp_score = trellis[tempv[j]][0].score;
			if (temp_lexicalTree[tempv[j]].childNode[0] == 0)
			{
				if (temp_score < mincost_leaf)
				{
					mincost_leaf = temp_score;
					mincost_leaf_index = tempv[j];
				}
			}

			if (mincost_next > temp_score)
			{
				mincost_next = temp_score;
			}
			
			ini_trellis_lookback(tempv[j], c, mincost, mincost_next, mincost_leaf, mincost_leaf_index);

		}

	}
	return;
}

void insertChar(int &insertindex, int &insertc, string s)
{
	vector<int> tempv = lexicalTree[insertindex].childNode;
	for (int j = 0; j != tempv.size(); j++)
	{
		int index = tempv[j];
		if (s[insertc] == lexicalTree[index].c && lexicalTree[index].childNode[0] != 0)
		{
			if (insertc == s.size() - 1)
			{
				return;
			}
			insertindex = tempv[j];
			insertChar(insertindex, ++insertc, s);
		}

	}
}

void readDictionary()
{
	ifstream in("F:\\研究生学习资料\\研一\\Speech Recognition Course\\project4\\dict_1.txt");
	if (!in)
	{
		cout << "No such file of dict_1.txt" << endl;
		exit(0);
	}
	Dict d;
	d.c = '*';
	lexicalTree.push_back(d);
	string s;

	while (in >> s)
	{

		int insertindex = 0;
		int insertc = 0;
		insertChar(insertindex, insertc, s);
		//insertChar(lexicalTree, insertindex, insertc, s);

		for (int i = insertc; i != s.size(); i++)
		{
			Dict tempD;
			tempD.c = s[i];
			//tempD.childNode.push_back(0);
			lexicalTree.push_back(tempD);
			int leafIndex = lexicalTree.size() - 1;
			lexicalTree[insertindex].childNode.push_back(leafIndex);
			insertindex = leafIndex;
			//lexicalTree[leafIndex].childNode.push_back(0);
		}
		lexicalTree[insertindex].childNode.push_back(0);

		wordLookUp tempWord;
		tempWord.word = s;
		tempWord.endindex = insertindex;
		dictionary.push_back(tempWord);

	}
	in.close();
}

void levenshtein_lexicalTree(string s, int &wordIndex)
{
	s = '*' + s;
	int m = lexicalTree.size();   //length of template
	int n = s.size();             //length of input
	for (int i = 0; i < m; i++)
		trellis[i].resize(n);
	int numOfWord = dictionary.size();


	for (int i = 1; i < n; i++)  //initial the first row
		trellis[0][i].score = i; 

	//Trellis tempT;                  //initial trellis' other state
	for (int i = 1; i < m; i++)
	{
		for (int j = 1; j < n; j++)
			trellis[i][j].score = MAXVALUE;
	}


	////without pruning 
	//for (int i = 1; i < n; i++)
	//{
	//	search_trellis(lexicalTree, rootindex, trellis, s, i);
	//}

	//with pruning
	temp_lexicalTree = lexicalTree;
	int mincost = 0;
	int mincost_next = MAXVALUE;
	for (int i = 1; i < n; i++)
	{
		bool dirty = false;
		search_trellis_pruning(0, s, i, mincost, mincost_next, dirty);
		mincost = mincost_next;
		mincost_next = MAXVALUE;
	}


	int min = MAXVALUE;
	for (int i = 0; i < numOfWord; i++)
	{
		int temp_min = trellis[dictionary[i].endindex][n - 1].score;
		//cout << "The word " << dictionary[i].word << "'s score is" << temp_min << endl;
		if (min > temp_min)
		{
			wordIndex = i;
			min = temp_min;
		}
	}
	//cout << "spell checker word is:" << dictionary[wordIndex].word << endl;

}

void levenshtein_lexicalTree_lookback(int n, char c, int &mincost, int &mincost_next, int &mincost_leaf, int &mincost_leaf_index)
{

	int m = lexicalTree.size();
	for (int i = 0; i < m; i++)
		trellis[i].resize(n + 1);

	trellis[0][n].score = mincost_leaf + BACKSCORE;
	for (int i = 1; i < m; i++) //initial line's other state
	{
		trellis[i][n].score = MAXVALUE;

	}
	trellis[0][n].j = mincost_leaf_index;
	trellis[0][n].i = n-1;
	
		
	mincost_next = MAXVALUE;
	mincost_leaf_index = 0;
	mincost_leaf = MAXVALUE;
	//search_trellis_lookback_nopruning(0, c, n, mincost, mincost_next, mincost_leaf, mincost_leaf_index);
	bool dirty = false;
	search_trellis_lookback_pruning(0, c, n, mincost, mincost_next, mincost_leaf, dirty, mincost_leaf_index);

}


void levenshtein_computeAccuracy(vector<string> s1, vector<string> s2, int flag, int &in, int &de, int &sub)
{
	int m = s1.size(); //length of template
	int n = s2.size(); //length of input
					   //cout << "m is:" << m << " " << "n is:" << n << endl;
	int cost = 0;
	int minvalue = 0; //the minimum value of the three paths
	int bestcost = 0; // 

	vector<vector<Trellis> > trellis(m); 	//creating the trellis table
	for (int i = 0; i < m; i++)
		trellis[i].resize(n);
	for (int i = 0; i < m; i++)    //initialing first row and column
	{
		trellis[i][0].score = i; trellis[i][0].valid = 0;
		trellis[i][0].deletion = trellis[i][0].insertion = trellis[i][0].substition = 0;
	}
	for (int i = 0; i < n; i++)
	{
		trellis[0][i].score = i; trellis[0][i].valid = 0;
		trellis[0][i].deletion = trellis[0][i].insertion = trellis[0][i].substition = 0;
	}

	for (int i = 1; i < n; i++)
	{
		int operation = 0;
		int invalid = 0;  //the number of invalid nodes
		for (int j = 1; j < m; j++)
		{
			trellis[j][i].valid = 1;
			if (!strcmp(s2[i].c_str(), s1[j].c_str()))
				cost = 0;
			else
			{
				cost = 1;
				//if (i == j)
				//cout << "s1 is:" << s1[i] << " " << "s2 is:" << s2[i] << endl;
			}
			//trellis[j][i] = min_value(trellis[j - 1][i - 1]+cost, trellis[j][i - 1]+1, trellis[j-1][i]+1);
			minvalue = trellis[j - 1][i - 1].score + cost;
			trellis[j][i].j = j - 1;
			trellis[j][i].i = i - 1;
			if (cost == 0) operation = 0; //0 means no alignment
			if (cost == 1) operation = 1; //1 means substition
			if (minvalue > trellis[j][i - 1].score + 1)
			{
				minvalue = trellis[j][i - 1].score + 1;
				trellis[j][i].j = j;
				trellis[j][i].i = i - 1;
				operation = 2; //2 means insertion
			}
			if (minvalue > trellis[j - 1][i].score + 1)
			{
				minvalue = trellis[j - 1][i].score + 1;
				trellis[j][i].j = j - 1;
				trellis[j][i].i = i;
				operation = 3; //3 means deletion
			}
			trellis[j][i].score = minvalue;
			if (flag == 0)
				absolutePathcost(trellis[j][i].score, trellis[j][i].valid);
			//cout << j << " " << i << " " << trellis[j][i].score << " " << trellis[j][i].i << " " << trellis[j][i].j << endl;
			switch (operation) {
			case 0:
				trellis[j][i].insertion = trellis[j - 1][i - 1].insertion;
				trellis[j][i].substition = trellis[j - 1][i - 1].substition;
				trellis[j][i].deletion = trellis[j - 1][i - 1].deletion;
				//cout << j << " " << i << endl;
				//cout << trellis[j][i].insertion << " " << trellis[j][i].substition << " " << trellis[j][i].deletion << endl;
				break;
			case 1:
				trellis[j][i].insertion = trellis[j - 1][i - 1].insertion;
				trellis[j][i].substition = trellis[j - 1][i - 1].substition + 1;
				trellis[j][i].deletion = trellis[j - 1][i - 1].deletion;
				//cout << j << " " << i << endl;
				//cout << trellis[j][i].insertion << " " << trellis[j][i].substition << " " << trellis[j][i].deletion << endl;
				break;
			case 2:
				trellis[j][i].insertion = trellis[j][i - 1].insertion + 1;
				trellis[j][i].substition = trellis[j][i - 1].substition;
				trellis[j][i].deletion = trellis[j][i - 1].deletion;
				//cout << j << " " << i << endl;
				//cout << trellis[j][i].insertion << " " << trellis[j][i].substition << " " << trellis[j][i].deletion << endl;
				break;
			case 3:
				trellis[j][i].insertion = trellis[j - 1][i].insertion;
				trellis[j][i].substition = trellis[j - 1][i].substition;
				trellis[j][i].deletion = trellis[j - 1][i].deletion + 1;
				//cout << j << " " << i << endl;
				//cout << trellis[j][i].insertion << " " << trellis[j][i].substition << " " << trellis[j][i].deletion << endl;
				break;
			}
		}
		if (flag == 1)
		{
			bestcost = trellis[0][i].score;
			for (int j = 1; j < m; j++)
			{
				if (bestcost > trellis[j][i].score)
					bestcost = trellis[j][i].score;
			}

			for (int j = 0; j < m; j++)
				beamSearch(trellis[j][i].score, trellis[j][i].valid, bestcost);
		}
		for (int j = 0; j < m; j++)
		{
			if (trellis[j][i].valid == 0)
				invalid++;
		}
		if (invalid == m)
		{
			cout << "over pruning";
			exit(0);
		}



	}
	in = trellis[m - 1][n - 1].insertion;
	de = trellis[m - 1][n - 1].deletion;
	sub = trellis[m - 1][n - 1].substition;
}