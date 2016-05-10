#include "create_trellis.h"
#include <algorithm>
#include <iostream>
#include <stack>
#include "HMMModel.h";
#include <vector>

using namespace std;


extern vector<vector<TreeMap> > firstpath_tree_Map;
extern vector<vector<TreeMap> > secondpath_tree_Map;
extern vector<vector<Trellis> > trellis;
extern vector<int> firstpath_lm_state_index;
extern vector<int> secondpath_lm_state_index;
extern vector<HMMModel*> models;
extern stack<int> result;


float min2(float a, float b, int &from)
{
	float  minvalue = a;
	if (minvalue > b)
	{
		minvalue = b;
		from = 2;
	}

	return minvalue;
}


float min3(float a, float b, float c, int &from)
{
	float  minvalue = a;
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

void trace_back(int m, int n)
{
	if (n == 0) 
		return;
	if (trellis[m][n].digit == 10)
	{
		cout << m << " " << n << " ";
		cout << "sil" << endl;
	}
	if (trellis[m][n].digit != -1 && trellis[m][n].digit != 10)
	{
		result.push(trellis[m][n].digit);
		//cout << m << " " << n << " " << trellis[m][n].score << " ";
	}
	trace_back(trellis[m][n].j, trellis[m][n].i);
	//cout << endl;
}

void creat_Map()
{
	for(int i = 0; i < FIRSTPATHSTATE -1; i++)
	{
		if (i == 0)
		{
			firstpath_tree_Map[i].resize(8);
			for (int j = 0; j < 8; j++)
			{
				firstpath_tree_Map[i][j].digit = j + 2;
				firstpath_tree_Map[i][j].valid = 1;
			}
		}
		else
		{
			firstpath_tree_Map[i].resize(10);
			for (int j = 0; j < 10; j++)
			{
				firstpath_tree_Map[i][j].digit = j;
				firstpath_tree_Map[i][j].valid = 1;
			}
		}
	}

	for (int i = 0; i < SECONDPATHSTATE - 1; i++)
	{
		secondpath_tree_Map[i].resize(10);
		for (int j = 0; j < 10; j++)
		{
			secondpath_tree_Map[i][j].digit = j;
			secondpath_tree_Map[i][j].valid = 1;
		}
	}
}

void init_trellis(int &m, int n)
{

	for (int i = 0; i < FIRSTPATHSTATE - 1; i++)
	{
		firstpath_lm_state_index.push_back(m);
		m++; //add lm state;
		m++; //add silence state;
		for (int j = 0; j < firstpath_tree_Map[i].size(); j++)
		{
			int digit = firstpath_tree_Map[i][j].digit;
			firstpath_tree_Map[i][j].index = m;   //save the first state's index
			for (int k = 0; k < models[digit]->getNumberOfStates(); k++)
			{
				m++;  //add hmm state
			}
		}
	}
	firstpath_lm_state_index.push_back(m);
	m++; // add the last state of the first path;
	int second_path_start_index = m;

	for (int i = 0; i < SECONDPATHSTATE - 1; i++)
	{
		secondpath_lm_state_index.push_back(m);
		m++;
		m++;
		for (int j = 0; j < secondpath_tree_Map[i].size(); j++)
		{
			int digit = secondpath_tree_Map[i][j].digit;
			secondpath_tree_Map[i][j].index = m;
			for (int k = 0; k < models[digit]->getNumberOfStates(); k++)
			{
				m++;
			}
		}
	}
	secondpath_lm_state_index.push_back(m);
	m++;   //add the last state of the second path

	trellis.resize(m);
	for (int i = 0; i < m; i++)
		trellis[i].resize(n);
	
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
				trellis[i][j].score = MAXVALUE;   //set all nodes MAXVALUE
				trellis[i][j].digit = -1;
		}
		
	}

	trellis[0][0].score = -log(1.0/2);          //initialize the first and fourth LM state
	trellis[second_path_start_index][0].score = -log(1.0 / 2);

}

void search(vector<vector<float> > mfcc39)
{
	int m = 0;
	int n = mfcc39.size();
	init_trellis(m, n);
	float firstpath_pre_mincost = 0;
	float firstpath_cur_mincost = 0;
	float secondpath_pre_mincost = 0;
	float secondpath_cur_mincost = 0;
	float pre_threshold;

	for (int i = 1; i < n; i++)   
	{
		firstpath_pre_mincost = firstpath_cur_mincost;
		firstpath_cur_mincost = MAXVALUE;
		pre_threshold = firstpath_pre_mincost + THRESHOLD;
		for (int j = 0; j < FIRSTPATHSTATE - 1; j++)  // first parth 0 to 7 lm_state
		{
			
			float value1, value2, value3;
			int from;
			int y;
			y = firstpath_lm_state_index[j];
			//if (j == 0)
			//{
			//	trellis[y][i].score = trellis[y + 1][i - 1].score;
			//	trellis[y][i].j = y + 1;
			//	trellis[y][i].i = i - 1;
			//}

			int lm_index = y;        //language model state index;
			float lm_state_cost = trellis[lm_index][i - 1].score; // set language model state cost
			y++;

			float silence_cost = models[10]->distanceMFCC(mfcc39[i], 1);   //cost of silence state
			value1 = trellis[y][i - 1].score + silence_cost;
			value2 = trellis[lm_index][i - 1].score + silence_cost + (-log(SILENCEPOSIBILITY*1.0));
			from = 1;
			trellis[y][i].score = min2(value1, value2, from);
			switch (from)
			{
			case 1:
				trellis[y][i].j = y;
				trellis[y][i].i = i - 1;
				break;
			case 2:
				trellis[y][i].j = lm_index;
				trellis[y][i].i = i - 1;
				break;
			}


			float prior_cost;
			if (j == 0) { prior_cost = -log(1.0 / 8 * (1-SILENCEPOSIBILITY)); }  //set prior cost
			else { prior_cost = -log(1.0 / 10 * (1-SILENCEPOSIBILITY)); }


			float digit_min_cost = MAXVALUE; //minimun leaf cost of all the digits
			int digit_min_cost_index = firstpath_tree_Map[j][0].index + models[firstpath_tree_Map[j][0].digit]->getNumberOfStates()-1; // the index of minumun leaf cost of all the digits
			int selected_digit = firstpath_tree_Map[j][0].digit;  // the initial guess digit
			//int digit_num = tree_Map[j].size(); // 8 or 10
			for (int k = 0; k < firstpath_tree_Map[j].size(); k++)
			{
				//bool isValid = false;            //set the vaild flag
				int digit = firstpath_tree_Map[j][k].digit; // 2 3 4 5 6...or 0 1 2 3 4...
				int index = firstpath_tree_Map[j][k].index;
				y = index;
				int numOfStates = models[digit]->getNumberOfStates();  //5
				for (int h = 1; h < numOfStates+1; h++) //from state 1 to state 5
				{
					float cost = models[digit]->distanceMFCC(mfcc39[i], h);
					float entry_cost;
					from = 1;

					if (h == 1)  //first state
					{
					    //??
						value1 = trellis[y][i - 1].score + models[digit]->getCost(1, 1) + cost;  // from horizontally left
						entry_cost = models[digit]->getCost(0, 1);
						value2 = lm_state_cost + prior_cost + entry_cost + cost;  //from diagonal
						if (value1 > pre_threshold) value1 = MAXVALUE;
						if (value2 > pre_threshold) value2 = MAXVALUE;
						if (value1 == MAXVALUE && value2 == MAXVALUE)
							//isValid = true;
							break;

						trellis[y][i].score = min2(value1, value2, from);
						if(trellis[y][i].score )
						switch (from)
						{
						case 1:
							trellis[y][i].j = y;
							trellis[y][i].i = i - 1;
							break;
						case 2:
							trellis[y][i].j = lm_index;
							trellis[y][i].i = i - 1;
							break;
						}
						y++;
					}
					else if (h == 2)
					{
						value1 = trellis[y][i - 1].score + models[digit]->getCost(h, h) + cost;
						value2 = trellis[y-1][i - 1].score + models[digit]->getCost(h-1, h) + cost;
						entry_cost = models[digit]->getCost(0, 2);
						value3 = lm_state_cost + prior_cost + entry_cost + cost;
						if (value1 > pre_threshold) value1 = MAXVALUE;
						if (value2 > pre_threshold) value2 = MAXVALUE;
						if (value3 > pre_threshold) value3 = MAXVALUE;
						if (value1 == MAXVALUE && value2 == MAXVALUE && value3 == MAXVALUE)
							//isValid = true;
							break;


						trellis[y][i].score = min3(value1, value2, value3, from);
						switch (from)
						{
						case 1:
							trellis[y][i].j = y;
							trellis[y][i].i = i - 1;
							break;
						case 2:
							trellis[y][i].j = y - 1;
							trellis[y][i].i = i - 1;
							break;
						case 3:
							trellis[y][i].j = lm_index;
							trellis[y][i].i = i - 1;
							break;
						}
						y++;
					}
					else
					{
						value1 = trellis[y][i - 1].score + models[digit]->getCost(h, h) + cost;
						value2 = trellis[y - 1][i - 1].score + models[digit]->getCost(h - 1, h) + cost;
						value3 = trellis[y - 2][i - 1].score + models[digit]->getCost(h - 2, h) + cost;
						if (value1 > pre_threshold) value1 = MAXVALUE;
						if (value2 > pre_threshold) value2 = MAXVALUE;
						if (value3 > pre_threshold) value3 = MAXVALUE;
						if (value1 == MAXVALUE && value2 == MAXVALUE && value3 == MAXVALUE)
							//isValid = true;
							break;

						trellis[y][i].score = min3(value1, value2, value3, from);
						switch (from)
						{
						case 1:
							trellis[y][i].j = y;
							trellis[y][i].i = i - 1;
							break;
						case 2:
							trellis[y][i].j = y - 1;
							trellis[y][i].i = i - 1;
							break;
						case 3:
							trellis[y][i].j = y - 2;
							trellis[y][i].i = i - 1;
							break;
						}
						y++;
					}
					if (trellis[y][i].score < firstpath_cur_mincost)   //calculate the current mincost
					{
						firstpath_cur_mincost = trellis[y][i].score;
					}
					if (h == numOfStates) 
					{
						if (trellis[y - 1][i].score < digit_min_cost)
						{
							digit_min_cost = trellis[y - 1][i].score;
							digit_min_cost_index = y - 1;
							selected_digit = digit;
						}
					}

				}  //end of state 5

				/*
				if (isValid == false)
				{
					vector<TreeMap>::iterator it = firstpath_tree_Map[j].begin() + k;
					cout << "you have cut first path: Node" << j << ", Digit:" <<firstpath_tree_Map[j][k].digit << endl;
					firstpath_tree_Map[j].erase(it);
					k--;
					continue;
				}*/


			} // end of lm state
			int next_lm_state_index = firstpath_lm_state_index[j + 1];
			trellis[next_lm_state_index][i].i = i;
			

			if (j != FIRSTPATHSTATE - 2)
			{
				float from_sil_cost = trellis[next_lm_state_index + 1][i - 1].score;
				if (from_sil_cost < digit_min_cost)
				{
					digit_min_cost = from_sil_cost;
					digit_min_cost_index = next_lm_state_index + 1;
					trellis[next_lm_state_index][i].i = i - 1;
					selected_digit = 10; //silence
				}
			}

			trellis[next_lm_state_index][i].score = digit_min_cost + ENDPENALTY;
			trellis[next_lm_state_index][i].j = digit_min_cost_index;
			trellis[next_lm_state_index][i].digit = selected_digit;

			if (trellis[next_lm_state_index][i].score >= MAXVALUE)
			{
				break;
			}

		}


		secondpath_pre_mincost = secondpath_cur_mincost;
		secondpath_cur_mincost = MAXVALUE;
		pre_threshold = secondpath_pre_mincost + THRESHOLD;
		for (int j = 0; j < SECONDPATHSTATE - 1; j++)  
		{
			float value1, value2, value3;
			int from;
			int y;
			y = secondpath_lm_state_index[j];
			//if (j == 0)
			//{
			//	trellis[y][i].score = trellis[y + 1][i - 1].score;
			//	trellis[y][i].j = y + 1;
			//	trellis[y][i].i = i - 1;
			//}

			int lm_index = y;        //language model state index;
			float lm_state_cost = trellis[lm_index][i - 1].score; // set language model state cost
			y++;

			float silence_cost = models[10]->distanceMFCC(mfcc39[i], 1);   //cost of silence state
			value1 = trellis[y][i - 1].score + silence_cost;
			value2 = trellis[lm_index][i - 1].score + silence_cost + (-log(SILENCEPOSIBILITY*1.0));
			from = 1;
			trellis[y][i].score = min2(value1, value2, from);
			switch (from)
			{
			case 1:
				trellis[y][i].j = y;
				trellis[y][i].i = i - 1;
				break;
			case 2:
				trellis[y][i].j = lm_index;
				trellis[y][i].i = i - 1;
				break;
			}


			float prior_cost = -log(1.0 / 10 * (1 - SILENCEPOSIBILITY));


			float digit_min_cost = MAXVALUE; //minimun leaf cost of all the digits
			int digit_min_cost_index = secondpath_tree_Map[j][0].index + models[secondpath_tree_Map[j][0].digit]->getNumberOfStates() - 1; // the index of minumun leaf cost of all the digits
			int selected_digit = secondpath_tree_Map[j][0].digit;  // the initial guess digit
																  //int digit_num = tree_Map[j].size(); // 8 or 10
			for (int k = 0; k < secondpath_tree_Map[j].size(); k++)
			{
				//bool isValid = false;            //set the vaild flag
				int digit = secondpath_tree_Map[j][k].digit; // 2 3 4 5 6...or 0 1 2 3 4...
				int index = secondpath_tree_Map[j][k].index;
				y = index;
				int numOfStates = models[digit]->getNumberOfStates();  //5
				for (int h = 1; h < numOfStates + 1; h++) //from state 1 to state 5
				{
					float cost = models[digit]->distanceMFCC(mfcc39[i], h);
					float entry_cost;
					from = 1;

					if (h == 1)  //first state
					{
						
						value1 = trellis[y][i - 1].score + models[digit]->getCost(1, 1) + cost;  // from horizontally left
						entry_cost = models[digit]->getCost(0, 1);
						value2 = lm_state_cost + prior_cost + entry_cost + cost;  //from diagonal
						if (value1 > pre_threshold) value1 = MAXVALUE;
						if (value2 > pre_threshold) value2 = MAXVALUE;
						if (value1 == MAXVALUE && value2 == MAXVALUE)
							//isValid = true;
							break;


						trellis[y][i].score = min2(value1, value2, from);
						switch (from)
						{
						case 1:
							trellis[y][i].j = y;
							trellis[y][i].i = i - 1;
							break;
						case 2:
							trellis[y][i].j = lm_index;
							trellis[y][i].i = i - 1;
							break;
						}
						y++;
					}
					else if (h == 2)
					{
						value1 = trellis[y][i - 1].score + models[digit]->getCost(h, h) + cost;
						value2 = trellis[y - 1][i - 1].score + models[digit]->getCost(h - 1, h) + cost;
						entry_cost = models[digit]->getCost(0, 2);
						value3 = lm_state_cost + prior_cost + entry_cost + cost;
						if (value1 > pre_threshold) value1 = MAXVALUE;
						if (value2 > pre_threshold) value2 = MAXVALUE;
						if (value3 > pre_threshold) value3 = MAXVALUE;
						if (value1 == MAXVALUE && value2 == MAXVALUE && value3 == MAXVALUE)
							//isValid = true;
							break;

						trellis[y][i].score = min3(value1, value2, value3, from);
						switch (from)
						{
						case 1:
							trellis[y][i].j = y;
							trellis[y][i].i = i - 1;
							break;
						case 2:
							trellis[y][i].j = y - 1;
							trellis[y][i].i = i - 1;
							break;
						case 3:
							trellis[y][i].j = lm_index;
							trellis[y][i].i = i - 1;
							break;
						}
						y++;
					}
					else
					{
						value1 = trellis[y][i - 1].score + models[digit]->getCost(h, h) + cost;
						value2 = trellis[y - 1][i - 1].score + models[digit]->getCost(h - 1, h) + cost;
						value3 = trellis[y - 2][i - 1].score + models[digit]->getCost(h - 2, h) + cost;
						if (value1 > pre_threshold) value1 = MAXVALUE;
						if (value2 > pre_threshold) value2 = MAXVALUE;
						if (value3 > pre_threshold) value3 = MAXVALUE;
						if (value1 == MAXVALUE && value2 == MAXVALUE && value3 == MAXVALUE)
							//isValid = true;
							break;

						trellis[y][i].score = min3(value1, value2, value3, from);
						switch (from)
						{
						case 1:
							trellis[y][i].j = y;
							trellis[y][i].i = i - 1;
							break;
						case 2:
							trellis[y][i].j = y - 1;
							trellis[y][i].i = i - 1;
							break;
						case 3:
							trellis[y][i].j = y - 2;
							trellis[y][i].i = i - 1;
							break;
						}
						y++;
					}

					if (trellis[y][i].score < secondpath_cur_mincost)   //calculate the current mincost
					{
						secondpath_cur_mincost = trellis[y][i].score;
					}

					if (h == numOfStates) 
					{
						if (trellis[y - 1][i].score < digit_min_cost)
						{
							digit_min_cost = trellis[y - 1][i].score;
							digit_min_cost_index = y - 1;
							selected_digit = digit;
						}
					}

				}  //end of state 5
				/*
				if (isValid == false)
				{
					vector<TreeMap>::iterator it = secondpath_tree_Map[j].begin() + k;
					cout << "you have cut second path: Node" << j << ", Digit:" << secondpath_tree_Map[j][k].digit << endl;
					secondpath_tree_Map[j].erase(it);
					
					k--;
					continue;
				}
				*/


			} // end of lm state
			int next_lm_state_index = secondpath_lm_state_index[j + 1];
			trellis[next_lm_state_index][i].i = i;
			//if (i == n - 1 && j == FIRSTPATHSTATE - 2)
			//{
			//	cout << endl;
			//}
			if (j != SECONDPATHSTATE - 2)
			{
				float from_sil_cost = trellis[next_lm_state_index + 1][i - 1].score;
				if (from_sil_cost < digit_min_cost)
				{
					digit_min_cost = from_sil_cost;
					digit_min_cost_index = next_lm_state_index + 1;
					trellis[next_lm_state_index][i].i = i - 1;
					selected_digit = 10; //silence

				}
			}
			trellis[next_lm_state_index][i].score = digit_min_cost + ENDPENALTY;
			trellis[next_lm_state_index][i].j = digit_min_cost_index;
			trellis[next_lm_state_index][i].digit = selected_digit;


			if (trellis[next_lm_state_index][i].score >= MAXVALUE)
			{
				break;
			}

		}

	}


	int firstpath_index = firstpath_lm_state_index[FIRSTPATHSTATE - 1];
	int secondpath_index = secondpath_lm_state_index[SECONDPATHSTATE - 1];
	float firstpath_cost = trellis[firstpath_index][n - 1].score;
	float secondpath_cost = trellis[secondpath_index][n - 1].score;

	int min_path_index = firstpath_index;
	if (secondpath_cost < firstpath_cost)
	{
		min_path_index = secondpath_index;
	}

	trace_back(min_path_index, n-1);

	cout << "You are dialing: ";
	while (!result.empty())
	{
		cout << result.top() << " ";
		result.pop();
	}
	
}


