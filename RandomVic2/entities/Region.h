#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;
class Prov;
class Region
{
public:
	string name;
	uint32_t ID;
	vector<Prov*> provinces;
	Region(string name, uint32_t ID);
	~Region();
};

