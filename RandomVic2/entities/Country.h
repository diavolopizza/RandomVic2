#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <random>
#include "Flag.h"
using namespace std;
class Prov;
class Region;
class Country
{
	ranlux24 random;

	string tag;
	uint32_t ID;
	Prov* capital;
	RGBTRIPLE colour;

public:
	Country(string tag, uint32_t ID, RGBTRIPLE colour, ranlux24 random);
	~Country();

	vector<Prov*> provinces;
	void addProvince(Prov *P);
	vector <Region*> regions;
	void addRegion(Region * R);
	RGBTRIPLE getColour();
	bool equalColour(RGBTRIPLE other);
	Flag * flag;
	uint32_t minX = MAXUINT32, minY = MAXUINT32;
	uint32_t maxX = 0, maxY = 0;

	uint32_t partyPopularities[4] = { 0,0,0,0 };





};

