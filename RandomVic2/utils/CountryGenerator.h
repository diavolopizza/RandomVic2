#pragma once
#include <iostream>
#include "../entities/Prov.h"
#include "../entities/Country.h"
#include "../utils/Bitmap.h"
#include <random>
#include "Data.h"
using namespace std;
class CountryGenerator
{
	ranlux48* random;
public:
	CountryGenerator(ranlux48* random);
	~CountryGenerator();

	vector<Country*> countries;
	void generateCountries(uint32_t amount);
	Country * GetClosestCountry(vector<Prov*> provinces, Prov * seekingProv);
	void distributeCountries(uint32_t amount, uint32_t sizeVariation, vector<Region*> regions);
	Bitmap * countryBMP(Bitmap * countryBMP);
};

