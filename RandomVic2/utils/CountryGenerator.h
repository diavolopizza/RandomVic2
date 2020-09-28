#pragma once
#include <iostream>
#include "../entities/Prov.h"
#include "../entities/Country.h"
#include "../utils/Bitmap.h"
#include "../terrain/Terrain.h"
#include <random>
#include <map>
#include "Data.h"
using namespace std;
class CountryGenerator
{
	ranlux24 random;
	Terrain *terrain;
public:
	CountryGenerator(Terrain *terrain);
	~CountryGenerator();
	map<string, Country*> countriesM;
	vector<Country*> countriesV;
	
	void generateCountries(uint32_t amount);
	Flag * createFlag();
	Country * GetClosestCountry(vector<Prov*> provinces, Prov * seekingProv);
	void distributeCountries(uint32_t amount, uint32_t sizeVariation, vector<Region*> regions);
	Bitmap countryBMP();
	Bitmap wealthBMP();
	Bitmap resourceBMP();
	Bitmap civilizationBMP();

	void determineDimensions();
	void sanityChecks();

};

