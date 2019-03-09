#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <random>
using namespace std;
class Prov;
class Region;
class Country
{
	ranlux48* random;

	string tag;
	uint32_t ID;
	Prov* capital;
	//Flag * flag;
	RGBTRIPLE colour;


public:
	Country(string tag, uint32_t ID, ranlux48* random);
	~Country();

	vector<Prov*> provinces;
	void addProvince(Prov *P);
	vector <Region*> regions;
	void addRegion(Region * R);
	RGBTRIPLE getColour();

};

