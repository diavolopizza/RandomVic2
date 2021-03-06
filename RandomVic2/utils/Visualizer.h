#pragma once
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "../utils/Data.h"
#include "../terrain/TerrainGenerator.h"
#include "../terrain/ProvinceGenerator.h"
using namespace std;
//using namespace cv;
class Visualizer
{
	ranlux24 random;

public:
	Visualizer();
	~Visualizer();
	//static void displayImage(Bitmap bitmap);
	//static void Visualizer::initializeWindow();
	void prettyRivers(Bitmap& riverBMP, const Bitmap heightma, const TerrainGenerator& terrainGeneratorp);
	void prettyContinents(Bitmap* continents, ProvinceGenerator& provinceGenerator);  //visualizer?
	void prettyRegions(Bitmap* regions, const ProvinceGenerator& provinceGenerator); //visualizer?

};

