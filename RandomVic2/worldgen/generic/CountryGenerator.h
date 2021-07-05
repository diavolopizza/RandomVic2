#pragma once
#include <iostream>
#include "../../entities/Province.h"
#include "../../entities/Country.h"
#include "../../utils/Bitmap.h"
#include "../../terrain/TerrainGenerator.h"
#include "../../terrain/ProvinceGenerator.h"
#include <random>
#include <map>
#include "../../utils/Data.h"
using namespace std;
class CountryGenerator
{
	ranlux24 random;
	ProvinceGenerator *provinceGenerator;
public:
	CountryGenerator(ProvinceGenerator *provinceGenerator);
	~CountryGenerator();
	map<string, Country*> countriesM;
	vector<Country*> countriesV;
	
	void generateCountries(uint32_t amount);
	Flag * createFlag();
	Country * GetClosestCountry(vector<Province*> provinces, Province * seekingProv);
	void distributeCountries(uint32_t amount, uint32_t sizeVariation);
	Bitmap countryBMP();
	Bitmap wealthBMP();
	Bitmap resourceBMP();
	Bitmap civilizationBMP();

	void determineDimensions();
	void sanityChecks();

};

