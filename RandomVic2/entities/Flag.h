#pragma once

#include <iostream>
#include <random>
#include <windows.h>
#include "../utils/libtarga.h"
using namespace std;
class Flag
{
	ranlux48* random;
	vector<RGBTRIPLE> colours;
	unsigned char* flag;


public:
	int width;
	int height;
	Flag(ranlux48*random);
	~Flag();
	void tricolore(int i, int j);
	void rotatedTricolore(int i, int j);
	void squareSquared(int i, int j);
	void circle(int i, int j);
	vector<RGBTRIPLE> generateColours();
	void setPixel(RGBTRIPLE colour, uint32_t x, uint32_t y);
	RGBTRIPLE getPixel(uint32_t x, uint32_t y);
	RGBTRIPLE getPixel(uint32_t pos);

};

