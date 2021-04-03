#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <random>
#include "Flag.h"
using namespace std;
class Province;
class Region;
class Country
{
	ranlux24 random;

	string tag;
	uint32_t ID;
	Province* capital;
	RGBTRIPLE colour;

public:
	Country(string tag, uint32_t ID, RGBTRIPLE colour, ranlux24 random);
	~Country();

	vector<Province*> provinces;
	void addProvince(Province *P);
	vector <uint32_t> regionIDs;
	void addRegion(uint32_t regionID);
	RGBTRIPLE getColour();
	bool equalColour(RGBTRIPLE other);
	Flag * flag;
	uint32_t minX = MAXUINT32, minY = MAXUINT32;
	uint32_t maxX = 0, maxY = 0;

	uint32_t partyPopularities[4] = { 0,0,0,0 };





};

