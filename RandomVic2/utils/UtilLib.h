#pragma once
using namespace std;
#include <algorithm>
#include <iostream>

static bool inRange(int offset, int high, int x)
{
	return ((x - (high - offset))*(x - offset) <= 0);
}
static double getDistance(int p1, int p2, int width, int height)
{
	const double x1 = p1 % width;
	const double x2 = p2 % width;
	const double y1 = (double)p1 / (double)height;
	const double y2 = (double)p2 / (double)height;
	return sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
}

class UtilLib
{

public:
	UtilLib();
	~UtilLib();
};

