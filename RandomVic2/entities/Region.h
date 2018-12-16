#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_set>
#include "Continent.h"
#include "Prov.h"
using namespace std;
//class Prov;
class Region
{
public:
	string name;
	uint32_t ID;
	vector<Prov*> provinces;	
	unordered_set<Region*> neighbourRegions;
	void setNeighbour(Region*R);
	Continent* continent;
	void assignContinent(Continent *C, uint32_t recursionDepth, uint32_t minProvPerContinent);
	Region(string name, uint32_t ID);
	~Region();
};

