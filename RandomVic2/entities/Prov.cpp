#include "Prov.h"


Prov::Prov()
{
}

Prov::Prov(int provnr,RGBTRIPLE colour)
{
	this->provnr = provnr;
	this->colour = colour;
	this->tag = "DUM";
}

Prov::Prov(int provnr, RGBTRIPLE colour,bool s, ranlux48* random)
{
	this->colour = colour;
	this->provnr = provnr;
	this->tag = "DUM";
	this->sea = s;
	this->continent = nullptr;
	this->region = nullptr;
	this->random = random;
	this->climate = "mild_climate";
}

Prov::~Prov()
{
}


bool Prov::operator==(const Prov& right) const
{
	
	if (this->colour.rgbtRed == right.colour.rgbtRed)
		if (this->colour.rgbtGreen == right.colour.rgbtGreen)
			if (this->colour.rgbtBlue == right.colour.rgbtBlue)
				return true;


	return false;

}



void Prov::setneighbour2(Prov*P)
{
	neighbourProvinces.insert(P);
	P->neighbourProvinces.insert(this);	
}

void Prov::checkDeveloped(vector <int> developed_continent) {
	for (auto dev_con : developed_continent)
	{
		//if (this->continent == dev_con)
		//{
		//	this->developed = true;
		//	break;
		//}
	}
}

void Prov::assignContinent(Continent * C)
{
	this->continent = C;
	C->provinces.push_back(this);
	for (auto neighbour : this->neighbourProvinces)
	{
		if (neighbour->continent == nullptr && !neighbour->sea) {
			neighbour->assignContinent(C);
		}
	}
}

void Prov::assignRegion(Region * R, bool recursive)
{
	this->region = R;
	R->provinces.push_back(this);
	if (recursive) {
		for (auto neighbour : this->neighbourProvinces)
		{
			if (neighbour->region == nullptr && !neighbour->sea && R->provinces.size() < 10) {
				neighbour->assignRegion(R, false);
			}
		}
	}
	if (R->provinces.size() < 10)
	{

	}
}
