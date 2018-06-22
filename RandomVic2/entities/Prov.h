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
	Prov(int, RGBTRIPLE);
	Prov(int, RGBTRIPLE, bool, ranlux48* random);
	~Prov();
	RGBTRIPLE colour;

	uint32_t provID;
	uint32_t center;
	Continent* continent;
	Region* region;
	bool placed = false, coastal = false, island = false, sea = false;
	bool developed = false;
	bool operator==(const Prov& right) const;
	string terrain_type;
	string climate;
	string owner;//tag who owns prov
	vector<string>cores;
	uint32_t lifeRating;
	string tradegood;

	unordered_set <Prov*> neighbourProvinces;//contains all the province Ids of the neighbouring provinces
	unordered_set<uint32_t> neighbourRegions;
	vector<uint32_t> pixels;
	unordered_set<uint32_t> positionCandidates;
	void setNeighbour(Prov*P);
	void checkDeveloped(vector <int> developed_continent);
	void assignContinent(Continent *C);
	void assignRegion(Region *R, bool recursive);
	void computeCandidates();
};

