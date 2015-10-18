#pragma once

struct Point {
	int x, y;
	Point();
	Point(int x, int y);
};

class Block
{
public:
	Point coord; // coordinates
	bool isHoriz; // direction
	int len; // length
	static int total; // used for id's
	int id; // unique id, goal == 0
	Block();
	Block(int x, int y, bool isHoriz, int len);
	Block(Point p, bool isHoriz, int len);
	~Block();
};
