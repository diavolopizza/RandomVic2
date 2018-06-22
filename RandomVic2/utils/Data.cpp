#include "Data.h"



Data::Data(uint32_t seed)
{
	this->random = new ranlux48();
	if (seed)
		random->seed(seed);
	else
		random->seed(time(NULL));
}


Data::~Data()
{
}
