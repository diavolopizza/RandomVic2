#pragma once
#include <vector>
#include <iostream>
#include <set>
#include <Windows.h>
#include "Continent.h"
#include "Region.h"
#include "Country.h"
#include <random>
using namespace std;

class Prov
{
	string path;
	ranlux24* random;
public:
	Prov();
	Prov(int, RGBTRIPLE);
	Prov(int, RGBTRIPLE, bool, ranlux24* random);
	~Prov();
	RGBTRIPLE colour;

	uint32_t provID = 0;
	uint32_t center = 0;
	Continent* continent = nullptr;
	Region* region = nullptr;
	Country * country = nullptr;
	bool placed = false, coastal = false, island = false, sea = false;
	bool operator==(const Prov& right) const;
	string terrain_type = "";
	string climate = "";
	string owner = "";//tag who owns prov
	vector<string>cores;
	float civilizationLevel;


	//VIC2
	uint32_t lifeRating;
	string tradegood;
	bool developed = false;
	void checkDeveloped(vector <int> developed_continent);

	vector <Prov*> neighbourProvinces;//contains all the province Ids of the neighbouring provinces
	vector<uint32_t> pixels;
	set<uint32_t> positionCandidates;
	void setNeighbour(Prov*P, bool level);
	void assignContinent(Continent *C);
	void assignRegion(Region *R, bool recursive, uint32_t minProvPerRegion);
	void computeCandidates();
};

