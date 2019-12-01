#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;
class Prov;
class Region;
class Continent
{

public:
	string name;
	uint32_t ID;
	bool civilized;
	vector<Prov*> provinces;
	vector<Region*> regions;
	Continent(string name, uint32_t ID);
	~Continent();
	void addProvince(Prov*P);
	void removeProvince(Prov *P);
};

