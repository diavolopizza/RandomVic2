#pragma once
#define D_SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
//#define NOMINMAX
#include <string>
#include <iostream>

#include <fstream>
#include <functional>
#include <algorithm>
#include <math.h>
#include <vector>
#include <random>
#include <boost/algorithm/clamp.hpp>
#include "TerrainGenerator.h"


using namespace std;

class ClimateGenerator
{
	ranlux24 random;

public:
	ClimateGenerator();
	~ClimateGenerator();

	//TERRAIN
	void humidityMap(Bitmap heightmapBMP, Bitmap* humidityBMP, uint32_t seaLevel, uint32_t updateThreshold);
	// visuals:
	void complexTerrain(Bitmap* terrainBMP, const Bitmap heightmap, uint32_t seaLevel, uint32_t updateThreshold);
};
