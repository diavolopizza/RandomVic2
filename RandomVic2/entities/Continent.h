#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;
class Province;
class Region;
class Continent
{

public:
	string name;
	uint32_t ID;
	bool civilized;
	vector<Province*> provinces;
	vector<Region*> regions;
	Continent(string name, uint32_t ID);
	~Continent();
	void addProvince(Province*P);
	void removeProvince(Province *P);
};

