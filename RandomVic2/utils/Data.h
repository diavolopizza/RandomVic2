#pragma once
#include <random>
#include <time.h>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace std;
class Data
{

public:
	Data();
	~Data();
	//Path params
	string VicPath = "";
	string mapSource = "";
	string modPath = "";
	string mapDest = "";
	string debugMapFolder = "map\\";


	//Map params
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t seed = 0;
	uint32_t seaLevel = 0;
	uint32_t seaProv = 0;
	uint32_t landProv = 0;
	uint32_t minProvPerContinent = 0;
	uint32_t minProvPerRegion = 0;
	uint32_t minProvSize = 0;

	//heightmap
	float fractalFrequency = 0;
	uint32_t fractalOctaves = 0;
	float fractalGain = 0;
	uint32_t borderLimiter = 10;//TODO


	//rivers
	uint32_t elevationTolerance = 0;
	uint32_t numRivers = 0;

	ranlux48* random;
	int maxNumOfCountries = 113;


	void getConfig(string configPath);
};

