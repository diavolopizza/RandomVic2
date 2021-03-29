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
#include <thread>
#include "../utils/Bitmap.h"
#include "../entities/Prov.h"
#include "../entities/River.h"
#include "../utils/Buffer.h"
#include "../utils/UtilLib.h"
#include "../utils/BMPHandler.h"
#include "../FastNoiseLite/Cpp/FastNoiseLite.h"
using namespace std;

class TerrainGenerator
{
	ranlux24 random;
	vector<vector<BYTE>> heightmapLayers;
	vector<unsigned char> randomValuesCached;

public:
	vector<River*> rivers;
	TerrainGenerator();
	~TerrainGenerator();

	//TERRAIN
	vector<BYTE> heightMap(uint32_t seed);
	vector<BYTE> normalizeHeightMap(Bitmap heightMap, vector<BYTE> worleyNoise);
	void createTerrain(Bitmap* terrainBMP, const Bitmap heightMapBmp);
	void worleyNoise(vector<BYTE> &layerValues, uint32_t width, uint32_t height);
	void detectContinents(Bitmap heightMap);
	void generateRivers(Bitmap* riverBMP, const Bitmap heightmap);
	void sanityChecks(Bitmap provinceBMP);
};
