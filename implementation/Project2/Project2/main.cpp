#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "levenshtein.h"
#include <time.h>

using namespace std;


int main()
{
	/*
	//calculate distance between two words
	string s1, s2;
	int flag;
	cout << "please input the template";
	cin >> s1;
	cout << "please input the word:";
	cin >> s2;
	cout << "please input the pruning strategies you want to use, 0 indicates fixed threshold, 1 indicates beam search";
	cin >> flag;
	levenshtein1(s1, s2, flag);
	*/

///////////////////////////////////////////////////////

	//compare a given text string to multiple templates
	/*
	string s1, s2;
	int flag;
	ifstream in("test.txt");
	if (!in)
	{
		cout << "No such file" << endl;
		exit(0);
	}
	string s;
	Dict dict[100];
	int numOfWords = 0;
	while(in >> s)
	{
		s1 = s1 + '$'+ s;
		dict[numOfWords].value = s;
		dict[numOfWords].length = s.size()+1;
		dict[numOfWords].valid = 1;
		numOfWords++;
	}
	in.close();
	cout << "please input the word:";
	cin >> s2;
	cout << "please input the pruning strategies you want to use, 0 indicates fixed threshold, 1 indicates beam search";
	cin >> flag;
	int index = 0;
	levenshtein2(s1, s2, flag, dict, numOfWords, index);
	cout << "the selected word is:";
	cout << dict[index].value;
	
	*/
	//////////////////////////////////

	/*
	//dict to story
	
	string s1, s2;
	int flag;
	cout << "please input the pruning strategies you want to use, 0 indicates fixed threshold, 1 indicates beam search";
	cin >> flag;
	ifstream in("dict.txt");
	if (!in)
	{
		cout << "No such file" << endl;
		exit(0);
	}
	string s;
	Dict dict[8000];
	int numOfWords = 0;
	time_t start, stop;
	start = time(NULL);
	while (in >> s)
	{
		
		s1 = s1 + '$' + s;
		//cout << s << " " << s.size() << endl;
		//cout << s1 << " " << s1.size() << endl;
		dict[numOfWords].value = s;
		dict[numOfWords].length = s.size() + 1;
		dict[numOfWords].valid = 1;
		numOfWords++;
	}
	in.close();
	ifstream in2("story.txt");
	if (!in2)
	{
		cout << "No such file" << endl;
		exit(0);
	}
	ofstream out("spellchecked.txt");
	int count = 0;
	while (in2 >> s2)
	{
		for (int i = 0; i < numOfWords; i++)
			dict[i].valid = 1;
		int index = 0;
		levenshtein2(s1, s2, flag, dict, numOfWords, index);
		//cout << index << endl;
		//cout << s2 << "->" << dict[index].value << endl;
		//cout << ++count << endl;
		out << dict[index].value << " ";		
	}
	in2.close();
	out.close();

	cout << "finished" << endl;
	stop = time(NULL);

	cout << "The total computation time is:" << stop - start << "s" << endl;

	*/
	////////////////////////////////////////////////////////////

	//single word to dict
	/*
	string s1, s2;
	int flag;
	cout << "please input the pruning strategies you want to use, 0 indicates fixed threshold, 1 indicates beam search";
	cin >> flag;
	ifstream in("dict.txt");
	if (!in)
	{
		cout << "No such file" << endl;
		exit(0);
	}
	string s;
	Dict dict[8000];
	int numOfWords = 0;
	while (in >> s)
	{

		s1 = s1 + '$' + s;
		//cout << s << " " << s.size() << endl;
		//cout << s1 << " " << s1.size() << endl;
		dict[numOfWords].value = s;
		dict[numOfWords].length = s.size() + 1;
		dict[numOfWords].valid = 1;
		numOfWords++;
	}
	in.close();

	cout << "please input the word:";
	cin >> s2;
	int index = 0;
	levenshtein2(s1, s2, flag, dict, numOfWords, index);
	cout << dict[index].value;
	while (1);
	*/
/////////////////////////////////////////////////////////////////
	
	/*
	 // compare two words, output the number of insertions, deletions and substution
	string s1, s2;
	int flag;
	cout << "please input the template";
	cin >> s1;
	cout << "please input the word:";
	cin >> s2;
	cout << "please input the pruning strategies you want to use, 0 indicates fixed threshold, 1 indicates beam search";
	cin >> flag;
	int in, de, sub;
	levenshtein3(s1, s2, flag, in, de, sub);
	cout << "total insertion is:" << in << endl;
	cout << "total delition is:" << de << endl;
	cout << "total substition is:" << sub << endl;
	*/


///////////////////////////////////////////////////////////
	//wrong version
	//calculate the total errors between spell-corrected version of the story and the correct story
	/*
	string s1, s2;
	int flag;
	cout << "please input the pruning strategies you want to use, 0 indicates fixed threshold, 1 indicates beam search";
	cin >> flag;
	ifstream in("story2.txt");
	ifstream in2("spellchecked3.txt");
	if (!in)
	{
		cout << "No such file" << endl;
		exit(0);
	}
	if (!in2)
	{
		cout << "No such file" << endl;
		exit(0);
	}
	
	int total_insertion = 0, total_deletion = 0, total_substution = 0;
	int count = 0;
	while (in >> s1)
	{
		in2 >> s2;
		//cout << s1 << " " << s2 << endl;
		int insertion = 0, deletion = 0, substution = 0;
		levenshtein3(s1, s2, flag, insertion, deletion, substution);
		//cout << s1 << " " << s2 << " insertion is:" << insertion << " deletion is:" << deletion << " substution is:" << substution << endl;
		//cout << ++count << endl;
		total_insertion += insertion;
		total_deletion += deletion;
		total_substution += substution;
	}
	in.close();
	in2.close();
	/*
	cout << "The total insertion is:" << total_insertion << endl;
	cout << "The total deletion is:" << total_deletion << endl;
	cout << "The total substution is:" << total_substution << endl;
	cout << "The total errors are:" << total_insertion + total_deletion + total_substution << endl;
	while(1);
	*/

////////////////////////////////////////
//remove all punctuation markers
	/*
	ifstream in("storycorrect.txt");
	if (!in)
	{
		cout << "No such file" << endl;
		exit(0);
	}
	ofstream out("storycorrect2.txt");
	char a;
	while (!in.eof())
	{
		a = in.get();
		//cout << a << " " << int(a);
		//system("pause");
		if (a >= 65 && a <= 90)
		{
			a = a + 32;
			out << a;
		}
		else if(a >= 97 && a <= 122 || a == 32)
			out << a;
	}
	out.close();
	in.close();
	*/

////////////////////////////////////////////////////////
	//calculate the total errors between spell-corrected version of the story and the correct story
	
	int flag;
	cout << "please input the pruning strategies you want to use, 0 indicates fixed threshold, 1 indicates beam search";
	cin >> flag;
	ifstream in("storycorrect.txt");
	ifstream in2("spellchecked.txt");
	if (!in)
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

	while (in >> s)
	{
		s1.push_back(s);
	}
	in.close();
	while (in2 >> s)
	{
		s2.push_back(s);
	}
	in2.close();

	int insertion = 0;
	int deletion = 0;
	int substution = 0;

	levenshtein4(s1, s2, flag, insertion, deletion, substution);

	cout << "The total insertion is:" << insertion << endl;
	cout << "The total deletion is:" << deletion << endl;
	cout << "The total substution is:" << substution << endl;
	cout << "The total errors are:" << insertion + deletion + substution << endl;

	

	while (1);
	
}