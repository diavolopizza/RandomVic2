#include "CountryGenerator.h"



CountryGenerator::CountryGenerator(ranlux48* random)
{
	this->random = random;
}


CountryGenerator::~CountryGenerator()
{
}



void CountryGenerator::generateCountries(uint32_t amount)
{
	for (int i = 0; i < amount; i++)
	{
		Country * C = new Country("GER", i, random);
		countries.push_back(C);
	}
}

Country * CountryGenerator::GetClosestCountry(vector<Prov*> provinces, Prov * seekingProv) {
	uint32_t distance = MAXUINT32;
	Country * C = nullptr;
	for (Prov* P : provinces)
	{
		if (!P->country)
			continue;
		uint32_t x1 = P->center % Data::getInstance().width;
		uint32_t x2 = seekingProv->center % Data::getInstance().width;
		uint32_t y1 = P->center / Data::getInstance().height;
		uint32_t y2 = seekingProv->center / Data::getInstance().height;
		if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
			distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
			C = P->country;
		}
	}
	return C;
}

void CountryGenerator::distributeCountries(uint32_t amount, uint32_t sizeVariation, vector<Region*> regions)
{
	generateCountries(amount);
	for (auto C : countries)
	{
		uint32_t regionIndex = (*random)() % regions.size();
		if (!regions[regionIndex]->country) {
			C->addRegion(regions[regionIndex]);
			regions[regionIndex]->setCountry(C);
		}/*

		vector<Region*> availableRegions;
		for (auto region : C->regions)
		{
			for (auto neighbourProv : region->neighbourRegions) {
				if (neighbourProv->country == nullptr)
				{
					availableRegions.push_back(neighbourProv);
				}
			}
		}*/
	}
	bool allAssigned = false;
	for (Region * R : regions)
	{
		if (R->country == nullptr)
		{
			for (auto neighbour : R->neighbourRegions)
			{
				if (neighbour->country != nullptr)
				{
					R->setCountry(neighbour->country);
					neighbour->country->addRegion(R);
					break;
				}
			}
		}
	}
	for (Region * R : regions)
	{
		if (R->country == nullptr)
		{
			uint32_t distance = MAXUINT32;
			Country* nextOwner = nullptr;
			for (Region* R2 : regions)
			{
				if (R2->country != nullptr) {
					uint32_t x1 = R2->provinces[0]->center  % Data::getInstance().width;
					uint32_t x2 = R->provinces[0]->center  % Data::getInstance().width;
					uint32_t y1 = R2->provinces[0]->center / Data::getInstance().height;
					uint32_t y2 = R->provinces[0]->center / Data::getInstance().height;
					if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
						distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
						nextOwner = R2->country;
					}
				}
			}
			R->setCountry(nextOwner);
			nextOwner->addRegion(R);
		}
	}
}

Bitmap * CountryGenerator::countryBMP(Bitmap * countryBMP) {
	for (auto country : countries)
	{
		for (auto region : country->regions)
			for (auto prov : region->provinces)
			{
				for (auto pixelIndex : prov->pixels)
				{
					countryBMP->setTripleAtIndex(country->getColour(), pixelIndex);
				}
			}
	}
	return countryBMP;
}
