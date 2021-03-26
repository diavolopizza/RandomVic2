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
	string debugMapsPath = "";


	//program flow params
	bool genHeight, genSimpleTerrain, genComplexTerrain, genV2, complexHeight, opencvVisualisation;


	//Map params
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t bitmapSize;
	uint32_t seed = 0;
	int seaLevel = 0;
	double landMassPercentage = 0;
	uint32_t seaProv = 0;
	uint32_t landProv = 0;
	uint32_t minProvPerContinent = 0;
	uint32_t minProvPerRegion = 0;
	uint32_t minProvSize = 0;

	//heightmap
	uint32_t layerAmount;
	vector<uint32_t> type;
	vector<double>fractalFrequency;
	vector<uint32_t> fractalOctaves;
	vector<double> fractalGain;
	vector<uint32_t> divideThreshold;
	vector<double> weight;
	vector<std::tuple<unsigned char, unsigned char>> heightRange;

	//land mass


	//humidity

	//rivers
	uint32_t elevationTolerance = 0;
	uint32_t numRivers = 0;

	//visualisation with openCV
	uint32_t updateThreshold = 0;

	//ranlux24* random;
	ranlux24 random2;
	int maxNumOfCountries = 20;
	uint32_t threadAmount = 8;


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

