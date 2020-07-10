#pragma once

#include <iostream>
#include <random>
#include <windows.h>
#include "../utils/libtarga.h"
using namespace std;
class Flag
{
	ranlux48* random;
public:
	int width;
	int height;
	unsigned char* flag;
	Flag(ranlux48*random);
	~Flag();
	void tricolore();
	void rotatedTricolore();
	void squareSquared();
	void circle();
	vector<RGBTRIPLE> generateColours();
	void setPixel(RGBTRIPLE colour, uint32_t x, uint32_t y);
	RGBTRIPLE getPixel(uint32_t x, uint32_t y);
	RGBTRIPLE getPixel(uint32_t pos);

};

