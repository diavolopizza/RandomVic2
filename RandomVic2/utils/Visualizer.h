#pragma once
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "../utils/Data.h"
#include "../terrain/Terrain.h"
using namespace std;
//using namespace cv;
class Visualizer
{


public:
	Visualizer();
	~Visualizer();
	//static void displayImage(Bitmap bitmap);
	//static void Visualizer::initializeWindow();
	void prettyRivers(Bitmap& riverBMP, const Bitmap heightma, const Terrain& terrainGeneratorp);

};

