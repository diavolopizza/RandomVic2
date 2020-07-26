#include "Colour.h"



Colour::Colour(int r, int g, int b)
{
	this->colour.rgbtRed = r;
	this->colour.rgbtGreen = g;
	this->colour.rgbtBlue = b;
}


Colour::~Colour()
{
}

bool Colour::operator==(const Colour &other) const
{
	if (other.colour.rgbtRed == colour.rgbtRed && other.colour.rgbtGreen == colour.rgbtGreen && other.colour.rgbtBlue == colour.rgbtBlue)
		return true;
	else
		return false;
}
