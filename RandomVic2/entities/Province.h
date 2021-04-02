#pragma once
#include <vector>
#include <iostream>
#include <set>
#include <Windows.h>
#include "Continent.h"
#include "Region.h"
#include "Country.h"
#include "..\utils\Data.h"
#include <random>
using namespace std;

class Province
{
	string path;
	ranlux24 random;
public:
	Province();
	Province(int, RGBTRIPLE);
	Province(int, RGBTRIPLE, bool);
	~Province();
	RGBTRIPLE colour;

	uint32_t provID = 0;
	uint32_t center = 0;
	Continent* continent = nullptr;
	Region* region = nullptr;
	Country * country = nullptr;
	bool placed = false, coastal = false, island = false, sea = false;
	bool operator==(const Province& right) const;
	string terrain_type = "";
	string climate = "";
	string owner = "";//tag who owns prov
	vector<string>cores;
	double civLevel;

	//VIC2
	uint32_t lifeRating;
	string tradegood;
	bool developed = false;
	void checkDeveloped(vector <int> developed_continent);

	vector <Province*> adjProv;//contains all the province Ids of the neighbouring provinces
	vector<uint32_t> pixels;
	vector<uint32_t> borderPixels;
	set<uint32_t> positionCandidates;
	void setNeighbour(Province*P, bool level);
	void assignContinent(Continent *C);
	void assignRegion(Region *R, bool recursive, uint32_t minProvPerRegion);
	void computeCandidates();
	bool hasAdjacent(Province*P);
};

