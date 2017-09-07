#include "Random.h"


//Random number generator wrapper
//Clinton Bale

#include "random.h"

Random::Random()
{
	Random((unsigned int)time(NULL));
}

Random::Random(unsigned int seed)
{
	srand(seed);
}

int Random::Next(int min, int max)
{
	return rand() % ((max + 1) - min) + min;
}

int Random::Next(int max)
{
	return Next(0, max);
}

int Random::Next()
{
	return Next(RAND_MAX);
}

double Random::NextDouble()
{
	return ((double)Next() / (double)RAND_MAX);
}

float Random::NextFloat()
{
	return (float)NextDouble();
}