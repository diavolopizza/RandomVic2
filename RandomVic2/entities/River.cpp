#include "River.h"



River::River()
{
}


River::~River()
{
}

void River::setSource(int pixel)
{
	this->source = pixel;
}

int River::getSource()
{
	return source;
}

int River::getCurrentEnd()
{
	return currentEnd;
}

void River::setcurrentEnd(int pixel)
{
	this->currentEnd = pixel;
}

bool River::contains(int pixel)
{
	for (auto pix : pixels)
	{
		if (pix == pixel)
			return true;
	}
	return false;
}

void River::addIngoing(River * ingoing, int pixel)
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

River * River::getIngoingForKey(int pixel)
{
	if (ingoingRivers.find(pixel) != ingoingRivers.end())
		return ingoingRivers.at(pixel);
	else return nullptr;
}
