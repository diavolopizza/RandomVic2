#include "Region.h"



void Region::setNeighbour(Region * R, bool level)
{
	bool found = false;
	for (auto neighbour : neighbourRegions)
	{
		if (R->ID == neighbour->ID)
			found = true;
	}
	if (!found) {
		neighbourRegions.push_back(R);
	}
	if (level)
		R->setNeighbour(this, false);
}

void Region::assignContinent(Continent * C, uint32_t recursionDepth, uint32_t minRegionPerContinent)
{
	uint32_t maxRegionPerContinent = 1000;
	if (recursionDepth < 5) {
		recursionDepth++;
		for (auto neighbour : this->neighbourRegions)
		{
			if (C->regions.size() < maxRegionPerContinent) {
				if (neighbour->continent == nullptr) {
					neighbour->assignContinent(C, recursionDepth, minRegionPerContinent);
				}
			}
			else
				break;
		}
	}
	for (Prov *prov : provinces)
	{
		prov->assignContinent(C);
	}
}

void Region::setCountry(Country * C)
{
	this->country = C;
	C->addRegion(this);
	for (auto prov : provinces) {
		prov->country = C;
		C->addProvince(prov);
	}
}

Region::Region(string name, uint32_t ID)
{
	this->name = name;
	this->ID = ID;
	this->continent = nullptr;
}


Region::~Region()
{
}
