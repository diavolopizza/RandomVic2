#include "Country.h"


Country::Country(string tag, uint32_t ID, ranlux48* random)
{
	this->tag = tag;
	this->ID = ID;
	this->random = random;
	this->colour = { 120 + (*random)() % 120, 120 + (*random)() % 120, 120 + (*random)() % 120 };
}

Country::~Country()
{
}

void Country::addProvince(Prov * P)
{
	this->provinces.push_back(P);
}

void Country::addRegion(Region * R)
{
	regions.push_back(R);
}

RGBTRIPLE Country::getColour()
{
	return colour;
}
