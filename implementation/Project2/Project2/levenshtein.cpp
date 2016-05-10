#include "levenshtein.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>

using namespace std;

#define MAX_VALUE 1E9



struct Trellis {
	int score;
	int i;
	int j;
	bool valid;
	int insertion;
	int deletion;
	int substition;
};


void showTheBestPath(vector<vector<Trellis> > trellis, int m, int n)
{
	if (m != 1 && n!= 1)
		showTheBestPath(trellis, trellis[m][n].j, trellis[m][n].i);
	cout << "(" << trellis[m][n].i << "," << trellis[m][n].j << ")->";
}

void showTheBestPath2(vector<vector<Trellis> > trellis, int m, int n, int k)
{
	if (m != k && n != 1)
		showTheBestPath2(trellis, trellis[m][n].j, trellis[m][n].i, k);
	cout << "(" << trellis[m][n].i << "," << (trellis[m][n].j)-k+1 << ")->";
}

void absolutePathcost(int &value, bool &valid)
{
	if (value > 3)
		valid = 0;
}

void beamSearch(int &value, bool &valid, int T)
{
	if (value > T + 3)
		valid = 0;
}

void display(vector<vector<Trellis> > trellis, int m, int n)
{
	cout << "Display the trellis: " << endl;
	for (int i = m - 1; i >= 0; i--)
	{
		for (int j = 0; j < n; j++)
			cout << trellis[i][j].score << " ";
		cout << endl;
	}
	cout << "Display the score:" << endl;
	cout << trellis[m - 1][n - 1].score << endl;

}

void displayValidity(vector<vector<Trellis> > trellis, int m, int n)
{
	cout << "Showing the validity of each nodes: " << endl;
	for (int i = m - 1; i >= 0; i--)
	{
		for (int j = 0; j < n; j++)
			cout << trellis[i][j].valid << " ";
		cout << endl;
	}
}



void levenshtein1(string s1, string s2, int flag)
{
	s1 = '$' + s1;
	s2 = '$' + s2;
	int m = s1.size(); //length of template
	int n = s2.size(); //length of input
	//cout << m << " " << n << endl;
	int cost = 0; 
	int minvalue = 0; //the minimum value of the three paths
	int bestcost = 0; // the minimum value of partial path


	vector<vector<Trellis> > trellis(m); 	//creating the trellis table
	for (int i = 0; i < m; i++)  	
		trellis[i].resize(n);
	for (int i = 0; i < m; i++)    //initialing first row and column
	{
		trellis[i][0].score = i; trellis[i][0].valid = 0;
	}
	for (int i = 0; i < n; i++)
	{
		trellis[0][i].score = i; trellis[0][i].valid = 0;
	}
	

	for (int i = 1; i < n; i++)
	{
		int invalid = 0;  //the number of invalid nodes
		for (int j = 1; j < m; j++)
		{
			trellis[j][i].valid = 1;
			if (s2[i] == s1[j])
				cost = 0;
			else
				cost = 1;
			//trellis[j][i] = min_value(trellis[j - 1][i - 1]+cost, trellis[j][i - 1]+1, trellis[j-1][i]+1);
			minvalue = trellis[j - 1][i - 1].score + cost;
			trellis[j][i].j = j - 1;
			trellis[j][i].i = i - 1;
			if (minvalue > trellis[j][i - 1].score + 1)
			{
				minvalue = trellis[j][i - 1].score + 1;
				trellis[j][i].j = j;
				trellis[j][i].i = i - 1;
			}
			if (minvalue > trellis[j - 1][i].score + 1)
			{
				minvalue = trellis[j-1][i].score + 1;
				trellis[j][i].j = j - 1;
				trellis[j][i].i = i ;
			}
			trellis[j][i].score = minvalue;
			if (flag == 0)
				absolutePathcost(trellis[j][i].score, trellis[j][i].valid);
			//cout << j << " " << i << " " << trellis[j][i].score << " " << trellis[j][i].i << " " << trellis[j][i].j << endl;

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


	display(trellis, m, n );
	cout << "Display the best path: " << endl;
	showTheBestPath(trellis, m-1, n-1);
	cout << "(" << n - 1 << "," << m - 1 << ")";
	displayValidity(trellis, m, n);

}

void levenshtein2(string s1, string s2, int flag, Dict dict[], int numOfWords, int &index)
{
	s2 = '$' + s2;
	int m = s1.size(); //length of template
	int n = s2.size(); //length of input
	int cost = 0;
	int minvalue = 0; //the minimum value of the three paths
	int bestcost = 0; // the minimum value of partial path

	vector<vector<Trellis> > trellis(m); 	//creating the trellis table
	for (int i = 0; i < m; i++)
		trellis[i].resize(n);

	int l = 0;;
	for (int num = 0; num < numOfWords; num++)
	{
		for (int i = 0; i < dict[num].length; i++)    //initialing first row and column
		{
			trellis[i+ l][0].score = i; trellis[i + l][0].valid = 0;
		}
		for (int i = 0; i < n; i++)
		{
			trellis[l][i].score = i; trellis[l][i].valid = 0;
		}
		l += dict[num].length;
	}

	int numofInvalid = 0;
	for (int i = 1; i < n; i++)
	{
		l = 0;
		for (int num = 0; num < numOfWords; num++)
		{
			if (dict[num].valid == 0)
			{
				l += dict[num].length;
				continue;
			}
			for (int j = 1; j < dict[num].length; j++)
			{
				trellis[j+l][i].valid = 1;
				if (s2[i] == s1[j+l])
					cost = 0;
				else
					cost = 1;
				minvalue = trellis[j+l-1][i-1].score + cost;
				trellis[j+l][i].j = j+l-1;
				trellis[j+l][i].i = i-1;
				if (minvalue > trellis[j+l][i-1].score + 1)
				{
					minvalue = trellis[j+l][i-1].score + 1;
					trellis[j+l][i].j = j+l;
					trellis[j+l][i].i = i-1;
				}
				if (minvalue > trellis[j+l-1][i].score + 1)
				{
					minvalue = trellis[j+l-1][i].score + 1;
					trellis[j+l][i].j = j+l - 1;
					trellis[j+l][i].i = i;
				}
				trellis[j+l][i].score = minvalue;
				if (flag == 0)
					absolutePathcost(trellis[j+l][i].score, trellis[j+l][i].valid);
			}
			l += dict[num].length;
		}
		if (flag == 1)
		{
			l = 0;
			bestcost = MAX_VALUE;
			for (int num = 0; num < numOfWords; num++)
			{
				if (dict[num].valid == 0)
				{
					l += dict[num].length;
					continue;
				}
				for (int j = 1; j < dict[num].length; j++)
				{
					if (bestcost > trellis[j+l][i].score)
						bestcost = trellis[j+l][i].score;
				}
				l += dict[num].length;
			}
			//cout << "The best score is" << bestcost << endl;
			
			l = 0;
			for (int num = 0; num < numOfWords; num++)
			{
				if (dict[num].valid == 0)
				{
					l += dict[num].length;
					continue;
				}
				for (int j = 1; j < dict[num].length; j++)
				{
					beamSearch(trellis[j+l][i].score, trellis[j+l][i].valid, bestcost);
				}
				l += dict[num].length;
			}

			/*for (int j = 0; j < m; j++)
				beamSearch(trellis[j][i].score, trellis[j][i].valid, bestcost);*/
		}

		l = 0;
		for (int num = 0; num < numOfWords; num++)
		{
			if (dict[num].valid == 0)
			{
				l += dict[num].length;
				continue;
			}
			
			int invalid = 0;
			for (int j = 0; j < dict[num].length; j++)
			{
				if (trellis[j+l][i].valid == 0)
					invalid++;
			}
			if (invalid == dict[num].length )
			{
				dict[num].valid = 0;
				numofInvalid++;
			}
			l += dict[num].length;
		}

		if (numofInvalid == numOfWords)
		{
			cout << "over pruning";
			exit(0);
		}
		//cout << numofInvalid << endl;

	}

	//int minPathCost = trellis[dict[0].length-1][n-1].score;
	int minPathCost = MAX_VALUE;
	l = 0;
	//l = dict[0].length;
	int indexOfWords = 0;
	for (int num = 0; num < numOfWords; num++)
	{
		if (dict[num].valid == 0)
		{
			l += dict[num].length;
			continue;
		}
		l += dict[num].length;
		if (minPathCost > trellis[l-1][n - 1].score)
		{
			minPathCost = trellis[l-1][n - 1].score;
			indexOfWords = num;
		}
	}
	//cout << indexOfWords;

	//cout << "Display the trellis: " << endl;
	//display(trellis, m, n);

	/*
	cout << "Display the selected trellis: " << endl;
	l = 0;
	for (int num = 0; num < indexOfWords; num++)
	{
		l += dict[num].length;
	}

	for (int i = l+ dict[indexOfWords].length - 1; i >= l; i--)
	{
		for (int j = 0; j < n; j++)
			cout << trellis[i][j].score << " ";
		cout << endl;
	}
	cout << "Display the score:" << endl;
	cout << trellis[l + dict[indexOfWords].length - 1][n - 1].score << endl;
	
	
	cout << "Display the best path: " << endl;
	showTheBestPath2(trellis, l + dict[indexOfWords].length - 1, n - 1, l+1);
	cout << "(" << n - 1 << "," << l + dict[indexOfWords].length  - 1 - l<< ")" << endl;

	*/
	//cout << "Display the validity: " << endl;
	//displayValidity(trellis, m, n);


	index = indexOfWords;
}

void levenshtein3(std::string s1, std::string s2, int flag, int &in, int &de, int &sub)
{
	
	s1 = '$' + s1;
	s2 = '$' + s2;
	int m = s1.size(); //length of template
	int n = s2.size(); //length of input
					   //cout << m << " " << n << endl;
	int cost = 0;
	int minvalue = 0; //the minimum value of the three paths
	int bestcost = 0; // the minimum value of partial path


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

	//cout << m << " " << n << endl;

	for (int i = 1; i < n; i++)
	{
		int operation = 0;
		int invalid = 0;  //the number of invalid nodes
		for (int j = 1; j < m; j++)
		{
			trellis[j][i].valid = 1;
			if (s2[i] == s1[j])
				cost = 0;
			else
				cost = 1;
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

void levenshtein4(vector<string> s1, vector<string> s2, int flag, int &in, int &de, int &sub)
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
			if (!strcmp(s2[i].c_str(),s1[j].c_str()))
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

