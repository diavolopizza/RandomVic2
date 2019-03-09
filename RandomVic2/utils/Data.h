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
	string debugMapFolder = "debugMap\\";


	//program flow params
	bool genHeight, genSimpleTerrain, genComplexTerrain, genV2, complexHeight, opencvVisualisation;


	//Map params
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t seed = 0;
	uint32_t seaLevel = 0;
	uint32_t landMassPercentage = 0;
	uint32_t seaProv = 0;
	uint32_t landProv = 0;
	uint32_t minProvPerContinent = 0;
	uint32_t minProvPerRegion = 0;
	uint32_t minProvSize = 0;

	//heightmap
	float fractalFrequency = 0;
	uint32_t fractalOctaves = 0;
	float fractalGain = 0;
	uint32_t divideThreshold = 0;//TODO

	//land mass


	//humidity



	//rivers
	uint32_t elevationTolerance = 0;
	uint32_t numRivers = 0;

	//visualisation with openCV
	uint32_t updateThreshold = 0;

	ranlux48* random;
	int maxNumOfCountries = 113;


	void getConfig(string configPath);

public:
	Data(Data const&) = delete;
	void operator=(Data const&) = delete;
	//variable info
	uint32_t amountOfRegions = 0;


	static Data& getInstance()
	{
		static Data instance; // Guaranteed to be destroyed.
		return instance;
	}
};

