#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <stdio.h>  
#include <vector>
#include <Windows.h>
#include <random>
#include "../utils/Bitmap.h"
#include "../entities/Prov.h"
#include "../terrain/VoronoiDiagramGenerator.h"
using namespace std;
class Terrain
{
	ranlux48_base rando;
	hash <int> prov_hash;//
	vector<Prov*> provinces;
public:
	Terrain();
	~Terrain();
	//void main(Various1*V);
	int filewidth = 5616;
	int fileheight = 2160;
	int filesize = filewidth*fileheight;
	BYTE* landProvinces(int numoflandprov, int width, int height, Bitmap * terrain, Bitmap * provinceBMP);
	BYTE* voronoi(int numoflandprov, int width, int height);


};
