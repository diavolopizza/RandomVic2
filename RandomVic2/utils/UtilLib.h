#pragma once
using namespace std;
#include <algorithm>

static bool inRange(int offset, int high, int x)
{
	return ((x - (high - offset))*(x - offset) <= 0);
}
static double getDistance(int p1, int p2, int width, int height)
{
	const int x1 = p1 % width;
	const int x2 = p2 % width;
	const int y1 = p1 / height;
	const int y2 = p2 / height;
	return sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
}

class UtilLib
{

public:
	UtilLib();
	~UtilLib();
};

