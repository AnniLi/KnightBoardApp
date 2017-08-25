#include "stdafx.h"
#include "KnightBoard.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

KnightBoard::KnightBoard(const pair<int, int>& boardSize, const string& mapFileName) {
	_boardSize = boardSize;
	createMap(mapFileName);
}

bool KnightBoard::isMoveValid(const pair<int, int>& startPoint, const pair<int, int>& endPoint) const {
	double distance = (startPoint.first - endPoint.first) * (startPoint.first - endPoint.first) +
		(startPoint.second - endPoint.second) * (startPoint.second - endPoint.second);
	if (distance != knightMoveDistance)
		return false;
	bool firstMoveValid = _barrierSquares.find({ startPoint.first, endPoint.second }) == _barrierSquares.end()
		&& _barrierSquares.find({ startPoint.first, (startPoint.second + endPoint.second) / 2 }) == _barrierSquares.end();
	bool secondMoveValid = _barrierSquares.find({ endPoint.first, startPoint.second }) == _barrierSquares.end()
		&& _barrierSquares.find({ endPoint.first, (startPoint.second + endPoint.second) / 2 }) == _barrierSquares.end();
	return firstMoveValid || secondMoveValid;
}

bool KnightBoard::isMovesValid(const vector<pair<int, int>>& moves, bool print) const {
	if (find_if(moves.begin(), moves.end(), [&](pair<int, int> point) {return !isPointValid(point); }) != end(moves))
		return false;
	for (auto ptr = moves.begin(); ptr != moves.end() - 1; ++ptr) 
		if (!isMoveValid(*ptr, *(ptr + 1)))
			return false;
	if (print)
		printMoves(moves);
	return true;
}

void KnightBoard::printMoves(const vector<pair<int, int>>& moves) const {
	auto startPoint = moves.front();
	auto endPoint = moves.back();
	for (auto it = moves.begin() + 1; it != moves.end() - 1; ++it) {
		for (int i = 0; i < _boardSize.first; ++i) {
			for (int j = 0; j < _boardSize.second; ++j) {
				if (i == startPoint.first && j == startPoint.second)
					cout << "S";
				else if (i == endPoint.first && j == endPoint.second)
					cout << "E";
				else if (i == (*it).first && j == (*it).second)
					cout << "K";
				else
					cout << ".";
			}
			cout << endl;
		}
		cout << endl;
	}
}

void KnightBoard::clear() {
	_points.clear();
	_viewedPoints.clear();
	_path.clear();
}


bool KnightBoard::isPointValid(const pair<int, int>& point) const {
	bool isValid = (point.first <= _boardSize.first - 1 && point.first >= 0 ) 
		&& (point.second <= _boardSize.second - 1 && point.second >= 0);
	isValid = isValid && (_rockSquares.end() == _rockSquares.find(point))
		&& (_barrierSquares.end() == _barrierSquares.find(point));
	return isValid;
}

int KnightBoard::findMaximumPath(const pair<int, int>& startPoint, const pair<int, int>& endPoint, vector<pair<int, int>>& path) {
	isMinimumMode = false;
	return findPath(startPoint, endPoint, path);
}

int KnightBoard::findMinimumPath(const pair<int, int>& startPoint, const pair<int, int>& endPoint, vector<pair<int, int>>& path) {
	isMinimumMode = true;
	return findPath(startPoint, endPoint, path);
}

int KnightBoard::findPath(const pair<int, int>& startPoint, const pair<int, int>& endPoint, vector<pair<int, int>>& path) {
	clear();
	path.clear();
	_points.insert(pair<int, pair<int, int>>(0, startPoint));
	while (!_points.empty()) {
		auto mininmum = _points.begin();
		_viewedPoints.insert(pair<pair<int, int>, int>((*mininmum).second, (*mininmum).first));
		auto currentStart = (*mininmum).second;
		auto currentLabel = (*mininmum).first;
		_points.erase(mininmum);
		createNextMoves(_points, currentStart, currentLabel);
	}
	auto result = _viewedPoints.find(endPoint);
	if (result != _viewedPoints.end()) {
		generatePath(startPoint, endPoint, path);
		return abs((*result).second);
	}
	return -1;
}

void KnightBoard::createNextMoves(multimap<int, pair<int, int>>& points, pair<int, int> startMove, int label) {
	std::vector<pair<int, int>> nextPoints;
	nextPoints.push_back({ startMove.first - 2, startMove.second + 1 });
	nextPoints.push_back({ startMove.first + 2, startMove.second + 1 });

	nextPoints.push_back({ startMove.first - 1, startMove.second + 2 });
	nextPoints.push_back({ startMove.first + 1, startMove.second + 2 });

	nextPoints.push_back({ startMove.first - 2, startMove.second - 1 });
	nextPoints.push_back({ startMove.first + 2, startMove.second - 1 });

	nextPoints.push_back({ startMove.first - 1, startMove.second - 2 });
	nextPoints.push_back({ startMove.first + 1, startMove.second - 2 });

	nextPoints.erase(remove_if(nextPoints.begin(), nextPoints.end(), [&](pair<int, int> point) {
		return !isPointValid(point) || _viewedPoints.find(point) != _viewedPoints.end() || !isMoveValid(startMove, point); }), nextPoints.end());

	for (auto& nextPoint : nextPoints) {				
		int newPrice = isMinimumMode ? minimumMovePrice(startMove, nextPoint) : maximumMovePrice(startMove, nextPoint) ;
		newPrice += label;

		if (_teleportSquares.find(nextPoint) != _teleportSquares.end())
			nextPoint = *find_if(_teleportSquares.begin(), _teleportSquares.end(), [nextPoint](pair<int, int> point) { return point != nextPoint; });

		auto it = find_if(points.begin(), points.end(), [&](pair<int, pair<int, int>> point) { return point.second == nextPoint; });
		if (it != end(points)) {
			if (newPrice < (*it).first) {
				points.insert(pair<int, pair<int, int>>(newPrice, (*it).second));
				_path.at((*it).second) = startMove;
				points.erase(it);
			}
		}
		else {
			points.insert(pair<int, pair<int, int>>(newPrice, nextPoint));
			_path.insert(pair<pair<int, int>, pair<int, int>>(nextPoint, startMove));
		}
	}
}

int KnightBoard::minimumMovePrice(const pair<int, int>& startPoint, const pair<int, int>& endPoint) {
	if (!isMoveValid(startPoint, endPoint))
		return invalidMove;
	if (_waterSquares.find(endPoint) != _waterSquares.end())
		return waterMoves;
	if (_lavaSquares.find(endPoint) != _lavaSquares.end())
		return lavaMoves;
	return 1;
}

int KnightBoard::maximumMovePrice(const pair<int, int>& startPoint, const pair<int, int>& endPoint) {
	if (!isMoveValid(startPoint, endPoint))
		return invalidMove;
	return -minimumMovePrice(startPoint, endPoint);
}

void KnightBoard::generatePath(const pair<int, int>& startPoint, const pair<int, int>& endPoint, vector<pair<int, int>>& path) {
	path.push_back(endPoint);
	while (path.back() != startPoint)
		path.push_back(_path.at(path.back()));
	reverse(path.begin(), path.end());
}

void KnightBoard::createMap(const string& fileName) {
	ifstream file(fileName);
	if (!file) {
		cout << "File " << fileName << " did not found.";
		return;
	}
	char symbol;
	int symbolPos= 0;
	pair<int, int> currentPoint;
	while (file.get(symbol)) {
		currentPoint = { symbolPos % _boardSize.first, symbolPos / _boardSize.first };
		switch (symbol)	{
		case 'R': 
			_rockSquares.insert(currentPoint);
			break;
		case 'L':
			_lavaSquares.insert(currentPoint);
			break;
		case 'W':
			_waterSquares.insert(currentPoint);
			break;
		case 'B':
			_barrierSquares.insert(currentPoint);
			break;
		case 'T':
			_teleportSquares.insert(currentPoint);
			break;
		case '\n':
			--symbolPos;
			break;
		default:
			break;
		}
		++symbolPos;
	}
}

KnightBoard::~KnightBoard() {
}
