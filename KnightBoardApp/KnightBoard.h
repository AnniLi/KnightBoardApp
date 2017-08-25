#pragma once
#include <utility>
#include <vector>
#include <map>
#include <set>

using std::pair;

class KnightBoard {
public:
	KnightBoard(const pair<int, int>& boardSize, const std::string& mapFileName);
	~KnightBoard();
	bool isMovesValid(const std::vector<pair<int, int>>& moves, bool print = false) const;

	int findMinimumPath(const pair<int, int>& startPoint, const pair<int, int>& endPoint, std::vector<pair<int, int>>& path);
	int findMaximumPath(const pair<int, int>& startPoint, const pair<int, int>& endPoint, std::vector<pair<int, int>>& path);

private:
	bool isMoveValid(const pair<int, int>& startPoint, const pair<int, int>& endPoint) const;
	bool isPointValid(const pair<int, int>& point) const;

	void createNextMoves(std::multimap<int, pair<int, int>>& points, pair<int, int> startPoint, int curentLabel);
	void generatePath(const pair<int, int>& startPoint, const pair<int, int>& endPoint, std::vector<pair<int, int>>& path);
	void createMap(const std::string& mapFileName);

	int minimumMovePrice(const pair<int, int>& startPoint, const pair<int, int>& endPoint);
	int maximumMovePrice(const pair<int, int>& startPoint, const pair<int, int>& endPoint);

	int findPath(const pair<int, int>& startPoint, const pair<int, int>& endPoint, std::vector<pair<int, int>>& path);
	void printMoves(const std::vector<pair<int, int>>& moves) const;

	void clear();

	pair<int, int> _boardSize = {8, 8};

	std::multimap<int, pair<int, int>> _points;
	std::map<pair<int, int>, int> _viewedPoints;
	std::map<pair<int, int>, pair<int, int>> _path;

	std::set<pair<int, int>> _rockSquares;
	std::set<pair<int, int>> _barrierSquares;
	std::set<pair<int, int>> _waterSquares;
	std::set<pair<int, int>> _lavaSquares;
	std::set<pair<int, int>> _teleportSquares;

	const int lavaMoves = 5;
	const int waterMoves = 3;
	const int knightMoveDistance = 5;
	bool isMinimumMode = true;
};

