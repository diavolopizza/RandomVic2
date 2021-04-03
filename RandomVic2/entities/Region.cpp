#include "Region.h"



void Region::setNeighbour(uint32_t regionID, bool level)
{
	bool found = false;
	for (auto neighbour : neighbourRegions)
	{
		if (regionID == neighbour)
			found = true;
	}
	if (!found) {
		neighbourRegions.push_back(regionID);
	}
	//if (level)
	//	R->setNeighbour(this, false);
}

void Region::assignContinent(Continent * C, uint32_t recursionDepth, uint32_t minRegionPerContinent)
{
	//uint32_t maxRegionPerContinent = 1000;
	//if (recursionDepth < 5) {
	//	recursionDepth++;
	//	for (auto neighbour : this->neighbourRegions)
	//	{
	//		if (C->regions.size() < maxRegionPerContinent) {
	//			if (neighbour->continent == nullptr) {
	//				neighbour->assignContinent(C, recursionDepth, minRegionPerContinent);
	//			}
	//		}
	//		else
	//			break;
	//	}
	//}
	//for (Province *prov : provinces)
	//{
	//	prov->assignContinent(C);
	//}
}

void Region::setCountry(Country * C)
{
	this->country = C;
	C->addRegion(this->ID);
	for (auto prov : provinces) {
		prov->country = C;
		C->addProvince(prov);
	}
}

Region::Region(string name, uint32_t ID)
{
	this->name = name;
	this->ID = ID;
}


Region::~Region()
{
}
