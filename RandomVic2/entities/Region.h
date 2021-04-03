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
	vector<uint32_t> neighbourRegions;
	void setNeighbour(uint32_t regionID, bool level);
	//Continent* continent = nullptr;
	uint32_t continentID;
	void assignContinent(Continent *C, uint32_t recursionDepth, uint32_t minProvPerContinent);
	void setCountry(Country * C);
	Region(string name, uint32_t ID);
	~Region();
};

