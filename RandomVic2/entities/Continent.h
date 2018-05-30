#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;
class Prov;
class Continent
{

public:
	string name;
	uint32_t ID;
	vector<Prov*> provinces;
	Continent(string name, uint32_t ID);
	~Continent();
	void addProvince(Prov*P);
	void removeProvince(Prov *P);
};

