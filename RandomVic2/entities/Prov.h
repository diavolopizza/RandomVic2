#pragma once
#include <vector>
#include <iostream>
#include <unordered_set>
#include <Windows.h>
#include "Continent.h"
#include "Region.h"
#include <random>
using namespace std;

class Prov
{
	string path;
	ranlux48* random;
public:
	Prov();
	Prov(int,RGBTRIPLE);
	Prov(int, RGBTRIPLE,bool, ranlux48* random);
	~Prov();


	string tag;//tag who owns prov
	uint32_t provnr;
	RGBTRIPLE colour;
	bool placed = false, coastal = false, island = false, sea = false;
	bool developed = false;
	Continent* continent;
	Region* region;
	bool operator==(const Prov& right) const;
	string terrain_type;
	string climate;
	uint32_t center;

	unordered_set <Prov*> neighbourProvinces;//contains all the province Ids of the neighbouring provinces
	unordered_set<uint32_t> neighbourRegions;
	vector<uint32_t > pixels;
	void setneighbour2(Prov*P);
	void checkDeveloped(vector <int> developed_continent);
	void assignContinent(Continent *C);
	void assignRegion(Region *R, bool recursive);
};

