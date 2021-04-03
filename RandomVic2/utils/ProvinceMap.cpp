#include "ProvinceMap.h"

ProvinceMap::ProvinceMap()
{
}

ProvinceMap::~ProvinceMap()
{
}

Province * ProvinceMap::operator[](const RGBTRIPLE colour)
{
	auto hash = to_hash(colour.rgbtRed, colour.rgbtGreen, colour.rgbtBlue);
	return provinceMap[hash];
}

void ProvinceMap::setValue(const RGBTRIPLE colour, Province * value)
{
	auto hash = to_hash(colour.rgbtRed, colour.rgbtGreen, colour.rgbtBlue);
	provinceMap[hash] = value;
}


