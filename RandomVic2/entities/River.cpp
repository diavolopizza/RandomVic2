#include "River.h"



River::River()
{
}


River::~River()
{
}

void River::setSource(uint32_t pixel)
{
	this->source = pixel;
	this->pixels.push_back(source);
}

uint32_t River::getCurrentEnd()
{
	return currentEnd;
}

void River::setcurrentEnd(uint32_t pixel)
{
	this->currentEnd = pixel;
}

bool River::contains(uint32_t pixel)
{
	for (auto pix : pixels)
	{
		if (pix == pixel)
			return true;
	}
	return false;
}
