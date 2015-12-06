// SudokuSolve.cpp : Quick C++ Library to Solve Sudoku Problems
//

#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <math.h>
#include <chrono>
using namespace std;
using ns = chrono::nanoseconds;
using get_time = chrono::steady_clock;
//Change Class
class Change {
public:
	int type;
	int x;
	int y;
	int value;
	Change(int _type, int _xcoord, int _ycoord, int _value) {
		type = _type;
		x = _xcoord;
		y = _ycoord;
		value = _value;
	}
	bool const operator== (const Change &i) const {
		return (i.type == type && i.x == x && i.y == y && i.value == value);
	}
	//friend bool operator== (Change &i, Change &j);
};
//Sudoku Class
class Sudoku {
public:
	int size;
	vector<vector<int>> workingset;
	vector<vector<vector<int>>> possibleset;
	vector<Change> updates;
	const bool slowmethod = true;
	const bool debug = false;
	//Function Implementations
	Sudoku(vector<vector<int>>& startmatrix)
	{

		size = startmatrix.size();
		if (size != (round(sqrt(size)) * round(sqrt(size)))) {
			cout << "Not a valid size - needs to be a perfect square!" << endl;
		}
		else {
			workingset = startmatrix;
			//Create a blank possibility set based on working set
			vector<vector<int>> addset;
			for (int i = 0; i < size; i++) {
				addset.push_back(vector<int>());
			}
			for (int i = 0; i < size; i++) {
				possibleset.push_back(addset);
			}
			auto start = get_time::now();
			PrintSudoku();
			cout << "solves to..." << endl;
			solveSudoku(workingset, possibleset);
			PrintSudoku();
			auto end = get_time::now();
			auto diff = end - start;
			if (debug) { cout << "Elapsed time is: " << chrono::duration_cast<ns>(diff).count() << "ns" << endl; };
		}
	}
	void refreshOptions(vector<vector<int>> &set, vector<vector<vector<int>>> &optionset) {
		for (size_t y = 0; y < set.size(); y++) {
			vector<int> row = set[y];
			for (size_t x = 0; x < row.size(); x++) {
				if (row[x] == 0) {
					if (!findOptions(x, y, set, optionset)) {
						reverseChoice(set, optionset);
					}
				}
			}
		}
	}
	bool findOptions(int x, int y, vector<vector<int>> &set, vector<vector<vector<int>>> &optionset) {
		bool output = true;
		vector<int> row = set[y];
		int value = row[x];
		if (value == 0) {
			//Array of possible values - can be progressively cancelled
			vector<int> range;
			for (int i = 1; i <= size; i++) {
				range.push_back(i);
			}
			//Loop over row
			for (int xx = 0; xx < size; xx++) {
				//Delete elements if already present
				auto it = find(range.begin(), range.end(), row[xx]);
				if (it != range.end()) {
					range.erase(it);
				}
			}
			//Loop over column
			for (int yy = 0; yy < size; yy++) {
				auto it = find(range.begin(), range.end(), (set[yy])[x]);
				if (it != range.end()) {
					range.erase(it);
				}
			}
			//Loop over "box"
			int yrd = roundDown(y, int(sqrt(size)));
			int xrd = roundDown(x, int(sqrt(size)));
			for (int yy = yrd; yy < (yrd + sqrt(size)); yy++) {
				for (int xx = xrd; xx < (xrd + sqrt(size)); xx++) {
					auto it = find(range.begin(), range.end(), (set[yy])[xx]);
					if (it != range.end()) {
						range.erase(it);
					}
				}
			}
			optionset[y][x] = range;
			//If range is one value update the set automatically
			if (range.size() == 1) {
				set[y][x] = range[0];
				if (debug) {
					cout << "Single value of " << range[0] << " inserted at " << x << ";" << y << endl;
				}
				updates.push_back(Change(0, x, y, range[0]));
			}
			if (range.size() == 0 && set[y][x] == 0) {
				if (debug) {
					cout << "No options at " << x << ";" << y << " - invalid set & will reverse changes" << endl;
				}
				output = false;
			}
		}
		return output;
	}
	void reverseChoice(vector<vector<int>> &set, vector<vector<vector<int>>> &optionset) {
		int lastindex = 0;
		//Gets last choice
		for (size_t start = 0; start < updates.size(); start++) {
			if ((updates[start]).type == 1) {
				lastindex = start;
			}
		}
		if (updates.size() > 0) {
			//Reset updates from end to choice
			if (debug) {
				cout << "Reversing " << updates.size() - lastindex << " updates" << endl;
			}
			for (size_t i = lastindex + 1; i < updates.size(); i++) {
				Change currentchange = updates[i];
				if (debug) {
					cout << "Reversing " << currentchange.value << " at " << currentchange.x << ";" << currentchange.y << endl;
				}
				set[currentchange.y][currentchange.x] = 0;
				//Updates possibility set (these values are possible again)
				optionset[currentchange.y][currentchange.x].push_back(currentchange.value);
			}
			updates.erase(updates.begin() + (lastindex + 1), updates.end());
			//Reset choice
			Change currentchange = updates[lastindex];
			vector<int> choicerange = optionset[currentchange.y][currentchange.x];
			//Remove Choice Range	
			choicerange.erase(remove(choicerange.begin(), choicerange.end(), currentchange.value), choicerange.end());
			if (choicerange.size() > 0) {
				//Pick other choice
				updates.pop_back();
				set[currentchange.y][currentchange.x] = choicerange[0];
				optionset[currentchange.y][currentchange.x] = choicerange;
				//Add new choice to update
				if (debug) {
					cout << "Now testing choice " << choicerange[0] << " on " << currentchange.x << ";" << currentchange.y << endl;
				}
				updates.push_back(Change(1, currentchange.x, currentchange.y, choicerange[0]));
			}
			else {
				//Move up in the tree again
				updates.pop_back();
				set[currentchange.y][currentchange.x] = 0;
				if (debug) {
					cout << "Reversing previous choice as no choices remain" << endl;
				}
				reverseChoice(set, optionset);
			}
		}
	}
	vector<Change> getLowestOptions(vector<vector<int>> &set, vector<vector<vector<int>>> &optionset) {
		vector<Change> options;
		for (size_t y = 0; y < optionset.size(); y++) {
			vector<vector<int>> row = optionset[y];
			for (size_t x = 0; x < row.size(); x++) {
				if (row[x].size() > 0 && set[y][x] == 0) {
					options.push_back(Change(2, x, y, row[x].size()));
				}
			}
		}
		sort(options.begin(), options.end(), [](const Change &i, Change &j) {
			return ((i.value < j.value) || (i.value == j.value && i.x < j.x) || (i.value == j.value && i.x == j.x && i.y < j.y)); });
		return options;
	}
	void solveSudoku(vector<vector<int>> &set, vector<vector<vector<int>>> &optionset) {
		refreshOptions(set, optionset);
		vector<Change> process = getLowestOptions(set, optionset);
		do {
			if (slowmethod) {
				for (size_t y = 0; y < set.size(); y++) {
					vector<int> row = set[y];
					for (size_t x = 0; x < row.size(); x++) {
						if (row[x] == 0) {
							//Enter a test choice!
							if (optionset[y][x].size() > 0) {
								set[y][x] = optionset[y][x][0];
								if (debug) {
									cout << "Testing choice " << optionset[y][x][0] << " on " << x << ";" << y << endl;
								}
								updates.push_back(Change(1, x, y, set[y][x]));
								if (!checkValid(set)) {
									reverseChoice(set, optionset);
								}
								refreshOptions(set, optionset);
							}
						}
					}
				}
			}
			else {
				if (process.size() > 0) {
					Change cur_choice = process[0];
					set[cur_choice.y][cur_choice.x] = optionset[cur_choice.y][cur_choice.x][0];
					if (debug) {
						cout << "Testing choice " << optionset[cur_choice.y][cur_choice.x][0] << " on " << cur_choice.x << ";" << cur_choice.y << endl;
					}
					updates.push_back(Change(1, cur_choice.x, cur_choice.y, set[cur_choice.y][cur_choice.x]));
					if (!checkValid(set)) {
						reverseChoice(set, optionset);
						refreshOptions(set, optionset);
						process = getLowestOptions(set, optionset);
					}
					refreshOptions(set, optionset);
					vector<Change> process_new = getLowestOptions(set, optionset);
				}
			}
		} while (!checkSolved(set));
	}
	//Useful Functions
	bool checkSolved(vector<vector<int>> set) {
		//Loops through row
		for (size_t y = 0; y < set.size(); y++) {
			vector<int> row = set[y];
			for (size_t x = 0; x < row.size(); x++) {
				int value = row[x];
				vector<int> range;
				for (size_t i = 1; i <= row.size(); i++) {
					range.push_back(i);
				}
				for (int xx = 0; xx < size; xx++) {
					auto it = find(range.begin(), range.end(), row[xx]);
					if (it != range.end()) {
						range.erase(it);
					}
				}
				for (int yy = 0; yy < size; yy++) {
					auto it = find(range.begin(), range.end(), (set[yy])[x]);
					if (it != range.end()) {
						range.erase(it);
					}
				}
				int yrd = roundDown(y, int(sqrt(size)));
				int xrd = roundDown(x, int(sqrt(size)));
				for (int yy = yrd; yy < (yrd + sqrt(size)); yy++) {
					for (int xx = xrd; xx < (xrd + sqrt(size)); xx++) {
						auto it = find(range.begin(), range.end(), (set[yy])[xx]);
						if (it != range.end()) {
							range.erase(it);
						}
					}
				}
				if (range.size() != 0) {
					return false;
				}
			}
		}
		return true;
	}
	bool checkValid(vector<vector<int>> set) {
		for (size_t y = 0; y < set.size(); y++) {
			vector<int> row = set[y];
			for (size_t x = 0; x < row.size(); x++) {
				int value = row[x];
				if (value == 0) {
					break;
				}
				for (int xx = 0; xx < size; xx++) {
					if (row[xx] == value && xx != x) {
						if (debug) {
							cout << "Invalid - multiple " << value << " on row " << y << endl;
						}
						return false;
					}
				}
				for (int yy = 0; yy < size; yy++) {
					if ((set[yy])[x] == value && yy != y) {
						if (debug) {
							cout << "Invalid - multiple " << value << " on column " << x << endl;
						}
						return false;
					}
				}
				int yrd = roundDown(y, int(sqrt(size)));
				int xrd = roundDown(x, int(sqrt(size)));
				for (int yy = yrd; yy < (yrd + sqrt(size)); yy++) {
					for (int xx = xrd; xx < (xrd + sqrt(size)); xx++) {
						if ((set[yy])[xx] == value && xx != x && yy != y) {
							if (debug) {
								cout << "Invalid - multiple " << value << " in box" << endl;
							}
							return false;
						}
					}
				}
			}
		}
		return true;
	}
	int roundDown(int val, int multiple) {
		if (multiple == 0 || val%multiple == 0) {
			return val;
		}
		else {
			return val - val%multiple;
		}
	}
	//Print Functions
	void PrintSudoku() {
		for (vector<int> i : workingset) {
			for (int j : i) {
				cout << j << "\t ";
			}
			cout << endl;
		}
	}
	void PrintPossibilities() {
		for (vector<vector<int>> i : possibleset) {
			for (vector<int> j : i) {
				cout << "[";
				if (j.size() > 0) {
					for (size_t k = 0; k < j.size() - 1; k++) {
						cout << j[k] << " ";
					}
					if (j.size() > 1) {
						cout << j[j.size() - 1];
					}
				}
				cout << "]";
			}
			cout << endl;
		}
	}
};
int _tmain(int argc, _TCHAR* argv[])
{
	//vector<vector<int>> input = { { 0, 1, 0, 4 }, { 2, 0, 0, 0 }, { 0, 0, 0, 3 }, { 1, 0, 4, 0 } };
	//vector<vector<int>> solution = { { 3, 1, 2, 4 }, { 2, 4, 3, 1 }, { 4, 2, 1, 3 }, { 1, 3, 4, 2 } };
	//vector<vector<int>> input =
	//{ { 0, 0, 0, 0, 0, 9, 0, 0, 0 }, { 0, 0, 0, 0, 0, 6, 2, 9, 0 }, { 0, 0, 3, 1, 5, 0, 0, 7, 0 },
	//{ 0, 0, 2, 0, 0, 0, 0, 1, 0 }, { 9, 0, 7, 0, 6, 0, 8, 0, 2 }, { 0, 1, 0, 0, 0, 0, 5, 0, 0 },
	//{ 0, 3, 0, 0, 1, 4, 9, 0, 0 }, { 0, 2, 6, 3, 0, 0, 0, 0, 0 }, { 0, 0, 0, 7, 0, 0, 0, 0, 0 } };
	
	 vector<vector<int>> input =
	{ { 8, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 3, 6, 0, 0, 0, 0, 0}, { 0, 7, 0, 0, 9, 0, 2, 0, 0 },
	{ 0, 5, 0, 0, 0, 7, 0, 0, 0 }, { 0, 0, 0, 0, 4, 5, 7, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0, 3, 0 },
	{ 0, 0, 1, 0, 0, 0, 0, 6, 8 }, { 0, 0, 8, 5, 0, 0, 0, 1, 0 }, { 0, 9, 0, 0, 0, 0, 4, 0, 0 } };
	
	Sudoku currentproblem = Sudoku(input);
	system("pause");
	return 0;
}