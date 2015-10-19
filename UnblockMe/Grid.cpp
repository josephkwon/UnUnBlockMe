#include "stdafx.h"
#include "Grid.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <queue>
#include <string>
#include <sstream>
#include <unordered_set>
#include <vector>

#include "Block.h"

ValidMoves::ValidMoves() : id(EMPTY), pos_allowance(0), neg_allowance(0) {}

ValidMoves::ValidMoves(int id, int pos, int neg) : id(id), pos_allowance(pos), neg_allowance(neg) {}

ValidMove::ValidMove() : id(EMPTY), move(0) {}

ValidMove::ValidMove(int id, int move) : id(id), move(move) {}

ValidState::ValidState(std::shared_ptr<std::vector<int>> state, int id, int move) : state(state), move(id, move) {}

Grid::Grid() : board(HEIGHT, std::vector<int>(6, -1)), output(std::cout), winningX(DEFAULT_WINNING_Y) {}

Grid::Grid(std::ostream& o) : board(HEIGHT, std::vector<int>(6, EMPTY)), output(o), winningX(DEFAULT_WINNING_Y) {}

Grid::~Grid() {}

void Grid::clearGrid()
{
	for (auto i = board.begin(); i != board.end(); ++i) {
		for (auto j = i->begin(); j != i->end(); ++j) {
			*j = EMPTY;
		}
	}
}

bool Grid::addBlock(int x, int y, bool isHoriz, int len)
{
	int x_offset(0);
	int y_offset(0);
	int& check(isHoriz ? x_offset : y_offset);
	bool good(true);
	for (int i = 0; i < len; ++i, ++check) {
		if (y + y_offset >= HEIGHT || x + x_offset >= WIDTH) {
			good = false;
			break;
		}
		if (board[y + y_offset][x + x_offset] >= 0) {
			good = false;
		}
	}
	if (good) {
		blocks.push_back(Block(x, y, isHoriz, len));
		int id = blocks[blocks.size() - 1].id;
		x_offset = 0;
		y_offset = 0;
		for (int i = 0; i < len; ++i, ++check) {
			board[y + y_offset][x + x_offset] = id;
		}
		return true;
	}
	return false;
}

void Grid::addBlocks(const std::string& filename)
{
	std::ifstream f(filename);
	int x, y, l;
	std::string line, s;
	while (std::getline(f, line)) {
		std::istringstream iss(line);
		if (iss >> x && iss >> y && iss >> s && iss >> l) {
			if (addBlock(x, y, (s != "v"), l)) {
				output << "Added " << (s == "v" ? "vertical" : "horizontal")
					<< " block at x = " << x << " y = " << y << " len = " << l << std::endl;
			}
			else {
				output << "Failed to add " << (s == "v" ? "vertical" : "horizontal")
					<< " block at x = " << x << " y = " << y << " len = " << l << std::endl;
			}
		}
		else {
			output << "Failed to parse line (" << line << ") correctly" << std::endl;
		}
	}
	if (blocks.size() > 0)
		winningX = WIDTH - blocks[0].len;
	else
		output << "No blocks added" << std::endl;
	output << std::endl;
}

void Grid::recordBlocks(std::vector<int>& coords)
{
	coords.clear();
	for (Block b : blocks) {
		coords.push_back(b.isHoriz ? b.coord.x : b.coord.y);
	}
}

void Grid::arrangeBlocks(std::vector<int>& coords)
{
	clearGrid();
	int x_offset;
	int y_offset;
	for (int i = 0; i < coords.size(); ++i) {
		if (blocks[i].isHoriz) {
			blocks[i].coord.x = coords[i];
		}
		else {
			blocks[i].coord.y = coords[i];
		}
		x_offset = blocks[i].coord.x;
		y_offset = blocks[i].coord.y;
		int& check(blocks[i].isHoriz ? x_offset : y_offset);
		check = coords[i];
		for (int j = 0; j < blocks[i].len; ++j, ++check) {
			board[y_offset][x_offset] = blocks[i].id;
		}
	}
}

uint Grid::getHash()
{
	uint hash(0);
	for (Block b : blocks) {
		hash += b.isHoriz ? b.coord.x : b.coord.y;
		hash <<= BITS_PER_BLOCK;
	}
	return hash;
}

uint Grid::getHash(const ValidMove& m)
{
	uint hash(0);
	for (Block b : blocks) {
		hash += b.isHoriz ? b.coord.x : b.coord.y;
		if (m.id == b.id)
			hash += m.move;
		hash <<= BITS_PER_BLOCK;
	}
	return hash;
}

void Grid::getValidMoves(std::vector<ValidMoves>& moves)
{
	for (Block b : blocks) {
		int x(b.coord.x), y(b.coord.y), x_offset(0), y_offset(0);
		int& check(b.isHoriz ? x_offset : y_offset);
		int MAX(b.isHoriz ? WIDTH : HEIGHT);
		int pos_allowance(0), neg_allowance(0);
		// check left and up
		while (--check >= -(b.isHoriz ? x : y) && board[y + y_offset][x + x_offset] < 0) {
			++neg_allowance;
		}
		check = b.len - 1;
		// check right and down
		while (++check < MAX - (b.isHoriz ? x : y) && board[y + y_offset][x + x_offset] < 0) {
			++pos_allowance;
		}
		if (neg_allowance > 0 || pos_allowance > 0)
			moves.push_back(ValidMoves(b.id, pos_allowance, neg_allowance));
	}
}

void Grid::doMove(const ValidMove& move)
{
	Block& b(blocks[move.id]);
	int &x(b.coord.x), &y(b.coord.y);
	int x_offset(0), y_offset(0);
	int& m(b.isHoriz ? b.coord.x : b.coord.y);
	int& check(b.isHoriz ? x_offset : y_offset);
	for (int i = 0; i < b.len; ++i, ++check) {
		board[y + y_offset][x + x_offset] = EMPTY;
	}
	m += move.move;
	check = 0;
	for (int i = 0; i < b.len; ++i, ++check) {
		board[y + y_offset][x + x_offset] = b.id;
	}
}

void Grid::undoMove(const ValidMove& move)
{
	Block& b(blocks[move.id]);
	int &x(b.coord.x), &y(b.coord.y);
	int x_offset(0), y_offset(0);
	int& m(b.isHoriz ? b.coord.x : b.coord.y);
	int& check(b.isHoriz ? x_offset : y_offset);
	for (int i = 0; i < b.len; ++i, ++check) {
		board[y + y_offset][x + x_offset] = EMPTY;
	}
	m -= move.move;
	check = 0;
	for (int i = 0; i < b.len; ++i, ++check) {
		board[y + y_offset][x + x_offset] = b.id;
	}
}

bool Grid::solve(std::vector<ValidMove>& path, std::unordered_set<uint>& done)
{
	return solveBFS(path, done);
}

bool Grid::solveDFS(std::vector<ValidMove>& path, std::unordered_set<uint>& done)
{
	if (isWin()) {
		return true;
	}
	std::vector<ValidMoves> moves;
	getValidMoves(moves);
	for (ValidMoves i : moves) {
		ValidMove k(i.id, 0);
		for (int z = 0; z < 2; ++z) {
			int limit(z == 0 ? i.pos_allowance : i.neg_allowance);
			for (int j = limit; j > 0; --j) {
				k.move = j * (z == 1 ? -1 : 1);
				uint hash(getHash(k));
				if (done.find(hash) != done.end()) {
					continue;
				}
				done.insert(hash);
				doMove(k);
				path.push_back(k);
				if (solveDFS(path, done)) {
					return true;
				}
				path.pop_back();
				undoMove(k);
			}
		}
	}
	return false;
}

void Grid::addValidMoves(std::queue<ValidState>& q, std::unordered_set<uint>& done, bool notInitial)
{
	std::vector<ValidMoves> moves;
	getValidMoves(moves);
	std::shared_ptr<std::vector<int>> currState = std::make_shared<std::vector<int>>(blocks.size());
	recordBlocks(*currState);
	for (ValidMoves i : moves) {
		ValidState k(currState, i.id, 0);
		if (notInitial) {
			k.path = q.front().path;
		}
		for (int z = 0; z < 2; ++z) {
			int limit(z == 0 ? i.pos_allowance : i.neg_allowance);
			for (int j = limit; j > 0; --j) {
				k.move.move = j * (z == 1 ? -1 : 1);
				uint hash(getHash(k.move));
				if (done.find(hash) != done.end()) {
					continue;
				}
				k.path.push_back(k.move);
				q.push(k);
				k.path.pop_back();
				done.insert(hash);
			}
		}
	}
}

bool Grid::solveBFS(std::vector<ValidMove>& path, std::unordered_set<uint>& done)
{
	std::queue<ValidState> q;
	addValidMoves(q, done, false);
	while (!q.empty()) {
		ValidState k(q.front());
		arrangeBlocks(*k.state);
		doMove(k.move);
		if (isWin()) {
			for (ValidMove m : k.path) {
				path.push_back(m);
			}
			return true;
		}
		addValidMoves(q, done, true);
		q.pop();
		undoMove(k.move);
	}
	return false;
}

bool Grid::isWin()
{
	if (blocks.size() <= 0)
		return true;
	return blocks[0].coord.x == winningX;
}

void Grid::printPath(std::vector<ValidMove>& path)
{
	for (auto i = path.crbegin(); i != path.crend(); ++i) {
		undoMove(*i);
	}
	for (auto i = path.cbegin(); i != path.cend(); ++i) {
		output << *this << std::endl;
		output << "Move Block " << i->id << " " << i->move << " slots." << std::endl;
		doMove(*i);
	}
	output << *this << std::endl;
}

std::ostream& operator<<(std::ostream& o, const Grid& g) {
	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			if (g.board[i][j] == 0) {
				o << "*";
			}
			else if (g.board[i][j] > 0 && g.board[i][j] < 10) {
				o << "0";
			}
			o << g.board[i][j] << " ";
		}
		o << std::endl;
	}
	return o;
}