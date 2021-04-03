#include "Continent.h"


Continent::Continent()
{
}

Continent::Continent(vector<uint32_t> continentPixels)
{
	pixels = continentPixels;
}

Continent::Continent(string name, uint32_t ID)
{
	this->name = name;
	this->ID = ID;
}

Continent::~Continent()
{
}

void Continent::addProvince(Province * P)
{
	provinces.push_back(P);
}

void Continent::removeProvince(Province * P)
{
}

bool Continent::findPixel(uint32_t pixel)
{
	return false;// pixels.find(pixel) != pixels.end();
}
