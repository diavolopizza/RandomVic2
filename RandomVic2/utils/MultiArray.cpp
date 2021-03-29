#include "MultiArray.h"



void MultiArray::resize(uint32_t dimension)
{
	provinceMap.resize(boost::extents[dimension][dimension][dimension]);
}

MultiArray::MultiArray()
{
}

MultiArray::~MultiArray()
{
}

Province * MultiArray::operator[](RGBTRIPLE colour)
{
	return provinceMap[colour.rgbtRed][colour.rgbtGreen][colour.rgbtBlue];
}

void MultiArray::setValue(RGBTRIPLE colour, Province * value)
{
	provinceMap[colour.rgbtRed][colour.rgbtGreen][colour.rgbtBlue] = value;
}


