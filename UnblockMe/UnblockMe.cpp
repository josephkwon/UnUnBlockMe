// UnblockMe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
using namespace std;

#include "Grid.h"

int solve(const string& input, const string& output) {
	ofstream out(output);
	Grid g(out);

	vector<ValidMove> path;
	unordered_set<uint> done;
	g.addBlocks(input);
	done.insert(g.getHash());

	bool solved(false);

	if (g.solve(path, done)) {
		g.printPath(path);
		solved = true;
	}
	else {
		out << "Failed to find solution." << endl;
	}
	out.close();
	return solved ? 0 : -2;
}

enum ARGV_CONSTANTS {
	PROGRAM_NAME,
	INPUT,
	OUTPUT,
	ARG_COUNT
};

int main(int argc, char* argv[])
{
	if (argc == INPUT) {
		return solve("in", "out");
	}
	else if (argc == OUTPUT) {
		string output("out");
		output += argv[INPUT];
		return solve(argv[INPUT], output);
	}
	else if (argc == ARG_COUNT) {
		return solve(argv[INPUT], argv[OUTPUT]);
	}
	else {
		cout << "Usage: " << argv[PROGRAM_NAME] << " <input> <output>" << endl;
		return -1;
	}
    return 0;
}

