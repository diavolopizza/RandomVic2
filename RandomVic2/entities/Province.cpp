#include "Province.h"


Province::Province()
{
}

Province::Province(int provID, RGBTRIPLE colour)
{
	this->provID = provID;
	this->colour = colour;
	this->owner = "DUM";
}

Province::Province(int provID, RGBTRIPLE colour, bool sea)
{
	this->colour = colour;
	this->provID = provID;
	this->owner = "DUM";
	this->sea = sea;
	this->regionID = 1000000; // unassigned
	this->continentID = 1000000; // unassigned
	this->random = Data::getInstance().random2;
	this->climate = "mild_climate";
}

Province::~Province()
{
}

bool Province::operator==(const Province& right) const
{

	if (this->colour.rgbtRed == right.colour.rgbtRed)
		if (this->colour.rgbtGreen == right.colour.rgbtGreen)
			if (this->colour.rgbtBlue == right.colour.rgbtBlue)
				return true;


	return false;

}

void Province::setNeighbour(Province*P, bool level = false)
{
	bool found = false;
	for (auto neighbour : adjProv)
	{
		if (P->provID == neighbour->provID)
			found = true;
	}
	if (!found) {
		adjProv.push_back(P);
	}
	if(level)
		P->setNeighbour(this, false);
}

void Province::checkDeveloped(vector <int> developed_continent) {
	for (auto dev_con : developed_continent)
	{
		//if (this->continent == dev_con)
		//{
		//	this->developed = true;
		//	break;
		//}
	}
}

void Province::assignContinent(Continent * C)
{
	////this->continent = C;
	//C->provinces.push_back(this);
	//for (auto neighbour : this->adjProv)
	//{
	//	if (neighbour->continent == nullptr && !neighbour->sea) {
	//		neighbour->assignContinent(C);
	//	}
	//}
}

void Province::assignRegion(uint32_t regionID, bool recursive, uint32_t minProvPerRegion, Region& R)
{
	this->regionID = regionID;
	R.provinces.push_back(this);

	if (recursive && R.provinces.size() < minProvPerRegion) {

		//for (auto neighbour : this->adjProv)
		//{
		//	if (neighbour->region == nullptr && !neighbour->sea && R->provinces.size() < minProvPerRegion) {
		//		neighbour->assignRegion(R, true, minProvPerRegion);
		//	}
		//}
		//vector<Prov*> neighboursVector;
		//for (auto neighbour : adjProv)
		//{
		//	if (!neighbour->sea)
		//		neighboursVector.push_back(neighbour);
		//}
		//Prov * randomNeighbour = neighboursVector[(*random)() % neighboursVector.size()];
		//randomNeighbour->assignRegion(R, true, minProvPerRegion);

		for (auto neighbour : this->adjProv)
		{
			if (neighbour->regionID == 1000000 && !neighbour->sea && R.provinces.size() < minProvPerRegion) {
				neighbour->assignRegion(R.ID, false, minProvPerRegion, R);
			}
		}
	}
	//if (R->provinces.size() < minProvPerRegion)
	//{

	//}
}

void Province::computeCandidates()
{
	while (positionCandidates.size() < 20 && positionCandidates.size() < pixels.size() - 1)
	{
		positionCandidates.insert(pixels[random() % pixels.size()]);
	}
}

bool Province::hasAdjacent(Province * P)
{
	for (auto adjProvince : adjProv)
	{
		if (adjProvince->provID == P->provID)
			return true;
	}
	return false;
}
