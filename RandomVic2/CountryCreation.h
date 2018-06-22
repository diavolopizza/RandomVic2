#pragma once
#include "entities/Prov.h"
class CountryCreation
{
public:
	CountryCreation();
	~CountryCreation();

	static void distributeCountries(vector<Prov*> provinces);
};

