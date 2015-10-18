#include "stdafx.h"
#include "Block.h"

int Block::total = 0;

Block::Block() : coord(Point()), isHoriz(true), len(2), id(total++) {}

Block::Block(int x, int y, bool isHoriz, int len) : coord(Point(x,y)), isHoriz(isHoriz), len(len), id(total++) {}

Block::Block(Point p, bool isHoriz, int len) : coord(p), isHoriz(isHoriz), len(len), id(total++) {}

Block::~Block() {}

Point::Point() : x(0), y(0) {}

Point::Point(int x, int y) : x(x), y(y) {}
