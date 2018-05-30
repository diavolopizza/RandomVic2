#include "Data.h"



Data::Data()
{
	this->random = new ranlux48();
	random->seed(time(NULL));
}


Data::~Data()
{
}
