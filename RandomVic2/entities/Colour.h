#pragma once
#include <iostream>
#include <random>
#include <windows.h>
using namespace std;
class Colour
{
	RGBTRIPLE colour;
	vector<Colour> incompatibles;

public:
	Colour(int, int, int);
	~Colour();
	bool operator==(const Colour &other) const;
};

