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
}

uint32_t River::getSource()
{
	return source;
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

void River::addIngoing(River * ingoing, uint32_t pixel)
{
	this->ingoing.push_back(ingoing);
	ingoing->addEndsIn(this);
	ingoingRivers.insert_or_assign(pixel, ingoing);
}

vector<River*> River::getIngoing()
{
	return ingoing;
}

void River::addEndsIn(River * river)
{
	this->endsIn = river;
}

River * River::getIngoingForKey(uint32_t pixel)
{
	if(ingoingRivers.find(pixel)!= ingoingRivers.end())
		return ingoingRivers.at(pixel);
	else return nullptr;
}
