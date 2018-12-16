#include "Region.h"



void Region::setNeighbour(Region * R)
{
	neighbourRegions.insert(R);
	R->neighbourRegions.insert(this);
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

Region::Region(string name, uint32_t ID)
{
	this->name = name;
	this->ID = ID;
	this->continent = nullptr;
}


Region::~Region()
{
}
