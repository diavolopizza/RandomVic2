#pragma once
#include <random>
#include <time.h>
using namespace std;
class Data
{

public:
	Data(uint32_t seed);
	~Data();

	string VicPath = "C:/Users/Paul/Documents/Visual Studio 2017/Projects/Victoriasource/";
	string mapPath = "C:/Users/Paul/Documents/Visual Studio 2017/Projects/map/";//;Victoriatarget/mod/ran/";
	string mapSource = "C:/Users/Paul/Documents/Visual Studio 2017/Projects/Victoriasource/map/";//;Victoriatarget/mod/ran/";
	string modPath = "C:/Users/Paul/Documents/Visual Studio 2017/Projects/Victoriatarget/mod/myMod/";

	ranlux48* random;
	int maxNumOfCountries = 113;
};

