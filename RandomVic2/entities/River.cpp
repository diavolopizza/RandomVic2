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
