#include "Country.h"


Country::Country(string tag, uint32_t ID, RGBTRIPLE colour, ranlux24* random)
{
	this->tag = tag;
	this->ID = ID;
	this->random = random;
	this->colour = colour;
	cout << tag << endl;

	for (auto pop : partyPopularities)
	{
		pop = (*random)() % 25;
	}

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

bool Country::equalColour(RGBTRIPLE other)
{
	if (colour.rgbtBlue == other.rgbtBlue)
		if (colour.rgbtGreen == other.rgbtGreen)
			if (colour.rgbtRed == other.rgbtRed)
				return true;
	return false;
}