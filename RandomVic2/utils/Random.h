//Random number generator wrapper
//Clinton Bale

#ifndef RANDOM_H
#define RANDOM_H

#include <ctime>
#include <cstdlib>

class Random
{
public:
	Random(unsigned int seed);
	Random();

	int Next(int min, int max);
	int Next(int max);
	int Next();
	double NextDouble();
	float NextFloat();
};

#endif// RANDOM_H
