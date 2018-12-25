#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <math.h>
#include <vector>
#include <Windows.h>
#include <random>
#include <unordered_set>
#include "../utils/Bitmap.h"
#include "../entities/Prov.h"
#include "../terrain/VoronoiDiagramGenerator.h"
#include "boost\multi_array.hpp"
#include "../entities/River.h"
#include "../utils/Buffer.h"
#include "../utils/Visualizer.h"

using namespace std;
//static array_type provinceMap(boost::extents[256][256][256]);
class Terrain
{
	ranlux48* random;
	hash <int> prov_hash;
	typedef boost::multi_array<Prov*, 3> array_type;
	typedef array_type::index index;
	array_type provinceMap;
	//boost::multi_array<Prov*, 3> provhashes();//reads definitions.csv and calculates the hashes of the rgb values, creates province with provnr,r,g,b,saves it in hashmap

public:
	vector<Prov*> provinces;
	vector<Region*> regions;
	vector<Continent*> continents;
	vector<River*> rivers;
	Terrain(ranlux48* random);
	~Terrain();

	//Utilities
	boost::multi_array<Prov*, 3> createProvinceMap();
	uint32_t Terrain::GetMinDistanceToProvince(uint32_t position, uint32_t width, uint32_t height);
	void determineStartingPixel(Bitmap* b, vector<uint32_t> &provincePixels, RGBTRIPLE &provinceColour, uint32_t provinceSize);
	void assignRemainingPixels(Bitmap * provinceBMP, BYTE* provinceBuffer, bool sea);
	void evaluateCoasts(Bitmap * provinceBMP);
	void evaluateNeighbours(Bitmap * provinceBMP);
	void provPixels(Bitmap * provinceBMP);
	//void assignColour(RGBTRIPLE provinceColour, BYTE* provinceBuffer, uint32_t currentPixel, uint32_t modifier = 1);
	BYTE* heightMap(Bitmap * terrainBMP, uint32_t seed, float frequency, uint32_t fractalOctaves, float fractalGain, uint32_t borderLimiter, uint32_t seaLevel, bool complexHeight, uint32_t updateThreshold);
	void createTerrain(Bitmap * terrainBMP, Bitmap * heightMapBmp, uint32_t & seaLevel, double landPercentage);
	BYTE* landProvinces(uint32_t numoflandprov, Bitmap * terrain, Bitmap * provinceBMP, uint32_t updateThreshold);
	BYTE* seaProvinces(uint32_t numOfSeaProv, uint32_t numoflandprov, Bitmap * terrain, Bitmap * provinceBMP, uint32_t updateThreshold);
	void provinceCreation(Bitmap * provinceBMP, uint32_t provinceSize, uint32_t numOfLandProv, uint32_t offset, uint32_t greyval);
	void fill(Bitmap* provinceBMP, uint32_t greyVal, uint32_t fillVal, uint32_t from, uint32_t to, vector<uint32_t> &randomValuesCached, uint32_t updateThreshold);
	void evaluateRegions(uint32_t minProvPerRegion, uint32_t width, uint32_t height);
	void prettyRegions(Bitmap * regions);
	void evaluateContinents(uint32_t minProvPerContinent, uint32_t width, uint32_t height);
	void prettyContinents(Bitmap * continents);
	void prettyProvinces(Bitmap * provinceBMP, uint32_t minProvSize);
	BYTE* humidityMap(Bitmap * heightmapBMP, Bitmap* humidityBMP, uint32_t seaLevel, uint32_t updateThreshold);
	void prettyTerrain(Bitmap * provinceBMP, Bitmap * heightmap, uint32_t seaLevel, uint32_t updateThreshold);
	void prettyRivers(Bitmap * riverBMP, Bitmap * heightmap, uint32_t riverAmount, uint32_t elevationTolerance, uint32_t seaLevel);
};
