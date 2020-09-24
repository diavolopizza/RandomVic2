#pragma once
#define D_SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#include <string>
#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <math.h>
#include <vector>
#include <Windows.h>
#include <random>
#include "../utils/Bitmap.h"
#include "../entities/Prov.h"
//#include "boost\multi_array.hpp"
#include "../utils/MultiArray.h"
#include "../entities/River.h"
#include "../utils/Buffer.h"
#include "../utils/Visualizer.h"

using namespace std;
//static array_type provinceMap(boost::extents[256][256][256]);
class Terrain
{
	ranlux48* random;
	hash <int> prov_hash;
	MultiArray provinceMap;
	vector<BYTE*> heightmapLayers;
	//boost::multi_array<Prov*, 3> provhashes();//reads definitions.csv and calculates the hashes of the rgb values, creates province with provnr,r,g,b,saves it in hashmap

public:
	vector<Prov*> provinces;
	vector<Region*> regions;
	vector<Continent*> continents;
	vector<River*> rivers;
	Terrain(ranlux48* random);
	~Terrain();

	//Utilities
	MultiArray createProvinceMap();
	uint32_t Terrain::GetMinDistanceToProvince(uint32_t position, uint32_t width, uint32_t height);
	void determineStartingPixel(Bitmap* b, vector<uint32_t> &provincePixels, RGBTRIPLE &provinceColour, uint32_t provinceSize);
	void assignRemainingPixels(Bitmap * provinceBMP, bool sea);
	void evaluateCoasts(Bitmap * provinceBMP);
	void evaluateNeighbours(Bitmap * provinceBMP);
	void provPixels(Bitmap * provinceBMP);
	BYTE* normalizeHeightMap(Bitmap * heightMap);
	BYTE* heightMap(Bitmap * terrainBMP, uint32_t seed, uint32_t &layer);
	void createTerrain(Bitmap * terrainBMP, const Bitmap * heightMapBmp);
	BYTE* landProvinces(uint32_t numoflandprov, Bitmap * terrain, Bitmap * provinceBMP, Bitmap* riverBMP, uint32_t updateThreshold);
	BYTE* seaProvinces(uint32_t numOfSeaProv, uint32_t numoflandprov, Bitmap * terrain, Bitmap * provinceBMP, Bitmap* riverBMP, uint32_t updateThreshold);
	void provinceCreation(Bitmap * provinceBMP, uint32_t provinceSize, uint32_t numOfLandProv, uint32_t offset, uint32_t greyval);
	void fill(Bitmap* provinceBMP, Bitmap* riverBMP, uint32_t greyVal, uint32_t fillVal, uint32_t from, uint32_t to, vector<uint32_t> &randomValuesCached, uint32_t updateThreshold);
	void evaluateRegions(uint32_t minProvPerRegion, uint32_t width, uint32_t height);
	void prettyRegions(Bitmap * regions);
	void evaluateContinents(uint32_t minProvPerContinent, uint32_t width, uint32_t height);
	void prettyContinents(Bitmap * continents);
	void prettyProvinces(Bitmap * provinceBMP, Bitmap* riverBMP, uint32_t minProvSize);
	BYTE* humidityMap(Bitmap * heightmapBMP, Bitmap* humidityBMP, uint32_t seaLevel, uint32_t updateThreshold);
	void prettyTerrain(Bitmap * provinceBMP, const Bitmap * heightmap, uint32_t seaLevel, uint32_t updateThreshold);
	void generateRivers(Bitmap * riverBMP, const Bitmap * heightmap);
	void prettyRivers(Bitmap* riverBMP, const Bitmap* heightmap);


	void sanityChecks(Bitmap * provinceBMP);
};
