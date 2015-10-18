#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "Block.h"

typedef unsigned long long uint;

enum CONSTANTS {
	DEFAULT_WINNING_Y = 2,
	HEIGHT = 6,
	WIDTH = 6,
	BITS_PER_BLOCK = 3
};

struct ValidMoves {
	int id;
	int pos_allowance, neg_allowance;
	ValidMoves();
	ValidMoves(int id, int pos, int neg);
};

struct ValidMove {
	int id;
	int move;
	ValidMove();
	ValidMove(int id, int move);
};

class Grid
{
public:
	std::ostream& output;
	std::vector<Block> blocks;
	std::vector<std::vector<int>> board;
	int winningX;
	Grid();
	Grid(std::ostream& o);
	~Grid();
	bool addBlock(int x, int y, bool isHoriz, int len);
	void addBlocks(const std::string& filename);
	uint getHash();
	uint getHash(const ValidMove& m);
	void getValidMoves(std::vector<ValidMoves>& moves);
	void doMove(const ValidMove& move);
	void undoMove(const ValidMove& move);
	bool solve(std::vector<ValidMove>& path, std::unordered_set<uint>& done);
	bool isWin();
	void printPath(std::vector<ValidMove>& path);
};

std::ostream& operator<<(std::ostream& o, const Grid& g);

