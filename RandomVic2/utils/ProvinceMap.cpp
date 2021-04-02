#include "ProvinceMap.h"

ProvinceMap::ProvinceMap()
{
}

ProvinceMap::~ProvinceMap()
{
}

Province * ProvinceMap::operator[](RGBTRIPLE colour)
{
	auto hash = to_hash(colour.rgbtRed, colour.rgbtGreen, colour.rgbtBlue);
	return provinceMap[hash];
}

void ProvinceMap::setValue(RGBTRIPLE colour, Province * value)
{
	auto hash = to_hash(colour.rgbtRed, colour.rgbtGreen, colour.rgbtBlue);
	provinceMap[hash] = value;
}


