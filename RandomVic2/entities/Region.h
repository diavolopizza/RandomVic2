#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include "Continent.h"
#include "Province.h"
#include "Country.h"
using namespace std;
class Region
{
public:
	string name;
	uint32_t ID;
	vector<Province*> provinces;	
	Country * country = nullptr;
	vector<Region*> neighbourRegions;
	void setNeighbour(Region*R, bool level);
	Continent* continent = nullptr;
	void assignContinent(Continent *C, uint32_t recursionDepth, uint32_t minProvPerContinent);
	void setCountry(Country * C);
	Region(string name, uint32_t ID);
	~Region();
};

