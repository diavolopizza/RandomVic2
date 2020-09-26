#pragma once

#include <iostream>
#include <random>
#include <windows.h>
#include "../utils/libtarga.h"

#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_point.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>
#include <boost/geometry.hpp>

namespace bg = boost::geometry;
using namespace std;
class Flag
{
	ranlux24* random;
	vector<RGBTRIPLE> colours;
	unsigned char* flag;
	enum type { TRICOLORE, ROTATEDTRICOLORE, PLAIN, };
	enum symbolType { CIRCLE, SQUARE, MOON, STAR, MOONSTAR, MULTISTAR };
	type flagType;
	symbolType symbolType;

public:
	int width;
	int height;
	Flag(ranlux24*random);
	~Flag();
	void tricolore(int i, int j);
	void rotatedTricolore(int i, int j);
	void plain(int i, int j);
	void squareSquared(int i, int j);
	void circle(int i, int j);
	void halfMoon(int i, int j );
	void star(int i, int j, float xPos, float yPos, float size);
	void halfMoonStars(int i, int j);
	//void triangle(int i, int j, )
	vector<RGBTRIPLE> generateColours();
	void setPixel(RGBTRIPLE colour, uint32_t x, uint32_t y);
	RGBTRIPLE getPixel(uint32_t x, uint32_t y);
	RGBTRIPLE getPixel(uint32_t pos);
	unsigned char* getFlag();
};

