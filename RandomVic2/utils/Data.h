#pragma once
#include <random>
#include <time.h>
using namespace std;
class Data
{

public:
	Data();
	~Data();

	string VicPath = "C:/Users/Paul/Documents/Visual Studio 2017/Projects/";//;Victoriasource/";
	string mapPath = "C:/Users/Paul/Documents/Visual Studio 2017/Projects/map/";//;Victoriatarget/mod/ran/";

	ranlux48* random;
	int maxNumOfCountries = 113;
};

