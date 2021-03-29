#pragma once
#include "../entities/Province.h"
class VictoriaCountryCreation
{
public:
	VictoriaCountryCreation();
	~VictoriaCountryCreation();

	static void distributeCountries(vector<Province*> provinces);
};

