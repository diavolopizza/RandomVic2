#include "Data.h"



Data::Data()
{
	this->random = new ranlux48();
	random->seed(4/*time(NULL)*/);
}


Data::~Data()
{
}
