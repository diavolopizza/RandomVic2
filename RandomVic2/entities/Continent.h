#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;
class Province;
class Region;
class Continent
{
	vector<uint32_t> pixels;
public:
	string name;
	uint32_t ID;
	bool civilized;
	vector<Province*> provinces;
	vector<Region*> regions;
	Continent();
	Continent(vector<uint32_t> continentPixels);
	Continent(string name, uint32_t ID);
	~Continent();
	void addProvince(Province*P);
	void removeProvince(Province *P);
	bool findPixel(uint32_t pixel);
};

