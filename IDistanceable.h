#pragma once
#include <vector>
#include <iostream>
using namespace std;
class IDistanceable
{
	uint32_t position;

public:
	IDistanceable();
	~IDistanceable();
	IDistanceable * findClosestObject(IDistanceable* source, vector<IDistanceable*> comparisonObjects, uint32_t &distance);
};

