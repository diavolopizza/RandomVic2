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
#include "../utils/Bitmap.h"
#include "../entities/Prov.h"
#include "../terrain/VoronoiDiagramGenerator.h"

using namespace std;

class Terrain
{
	ranlux48* random;
	hash <int> prov_hash;
	vector<Prov*> provinces;
public:
	Terrain(ranlux48* rando);
	~Terrain();

	//Utilities
	void determineStartingPixel(uint32_t bmpSize, uint32_t bmpWidth, BYTE* provincebuffer, vector<uint32_t> &provincePixels, RGBTRIPLE provinceColour);
	void assignRemainingPixels(uint32_t bmpSize, BYTE* provinceBuffer, vector<Prov*> provinces, uint32_t bmpWidth, uint32_t bmpHeight);
	void assignColour(RGBTRIPLE provinceColour, BYTE* provinceBuffer, uint32_t currentPixel);
	BYTE* landProvinces(uint32_t numoflandprov, Bitmap * terrain, Bitmap * provinceBMP);
	void provinceFiles();
	BYTE* voronoi(uint32_t numoflandprov, uint32_t width, uint32_t height);
};
