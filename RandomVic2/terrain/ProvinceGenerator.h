#pragma once
#define D_SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#include "TerrainGenerator.h"
#include "../utils/ProvinceMap.h"
#include "../entities/Province.h"
#include <thread>
using namespace std;

class ProvinceGenerator
{
	ranlux24 random;
	hash <int> prov_hash;
	ProvinceMap provinceMap;
	vector<unsigned char> randomValuesCached;

public:
	vector<Province*> provinces;
	vector<Region*> regions;
	vector<Continent*> continents;
	ProvinceGenerator();
	~ProvinceGenerator();
	vector<BYTE> landProvinces(Bitmap terrain, Bitmap* provinceBMP, Bitmap riverBMP, uint32_t updateThreshold);
	void determineStartingPixel(Bitmap* b, vector<uint32_t> &provincePixels, RGBTRIPLE &provinceColour, uint32_t provinceSize);
	void provinceCreation(Bitmap* provinceBMP, uint32_t provinceSize, uint32_t numOfProvs, uint32_t offset, uint32_t greyval);
	void fill(Bitmap* provinceBMP, const Bitmap riverBMP, const unsigned char greyVal, const unsigned char fillVal, const uint32_t from, const uint32_t to, uint32_t updateThreshold);
	void beautifyProvinces(Bitmap* provinceBMP, Bitmap riverBMP, uint32_t minProvSize);
	ProvinceMap createProvinceMap();
	int GetMinDistanceToProvince(uint32_t position, uint32_t width, uint32_t height);
	void evaluateCoasts(Bitmap provinceBMP);
	void evaluateNeighbours(Bitmap provinceBMP);
	void provPixels(const Bitmap* provinceBMP);
	void assignRemainingPixels(Bitmap* provinceBMP, bool sea);




	// regions
	void evaluateRegions(uint32_t minProvPerRegion, uint32_t width, uint32_t height);
	//continentes
	void evaluateContinents(uint32_t minProvPerContinent, uint32_t width, uint32_t height);
};
