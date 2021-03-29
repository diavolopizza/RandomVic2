#include "CountryCreation.h"



CountryCreation::CountryCreation()
{
}


CountryCreation::~CountryCreation()
{
}

void CountryCreation::distributeCountries(vector<Province*> provinces)
{
	for (auto prov : provinces) {
		if (prov->sea)
		{
			continue;
		}
		prov->owner = "GER";
		prov->cores.push_back("GER");
		prov->lifeRating = 40;
		prov->tradegood = "cattle";
	}
}
