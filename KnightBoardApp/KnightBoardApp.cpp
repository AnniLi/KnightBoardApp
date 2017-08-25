// KnightBoardApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <ostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include "KnightBoard.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[]) {
	string mapFileName = "D:/map.txt";
	if (argc > 1) {
		mapFileName = *argv[1];// вывод второй строки из массива указателей на строки(нумерация в строках начинается с 0 )
	}
	KnightBoard board({ 32, 32 }, mapFileName);
	vector<pair<int, int>> result;
	cout << board.findMaximumPath({ 1, 2 }, { 5, 4 }, result) << endl;
	//for_each(result.begin(), result.end(), [](pair<int, int> point) {cout << "(" << point.first << "," << point.second << ")"; });
	cout << endl;
	board.isMovesValid(result);

	cout << board.findMinimumPath({ 1, 2 }, { 5, 4 }, result) << endl;
	//for_each(result.begin(), result.end(), [](pair<int, int> point) {cout << "(" << point.first << "," << point.second << ")"; });
	cout << endl;
	board.isMovesValid(result, true);
	return 0;
}
