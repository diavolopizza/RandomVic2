#pragma once
#include "../entities/Province.h"
class CountryCreation
{
public:
	CountryCreation();
	~CountryCreation();

	static void distributeCountries(vector<Province*> provinces);
};

