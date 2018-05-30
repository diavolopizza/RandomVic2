#include "Continent.h"


Continent::Continent(string name, uint32_t ID)
{
	this->name = name;
	this->ID = ID;
}

Continent::~Continent()
{
}

void Continent::addProvince(Prov * P)
{
	provinces.push_back(P);
}

void Continent::removeProvince(Prov * P)
{
}