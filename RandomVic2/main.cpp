#include "utils/BMPHandler.h"
#include "terrain\Terrain.h"
#include <random>



using namespace std;

int main() {
	Terrain t;
	const char* provsource = "C:/Users/Paul/Documents/Visual Studio 2017/Projects/Victoriasource/map/provinces.bmp";
	const char* terrainsource = "C:/Users/Paul/Documents/Visual Studio 2017/Projects/Victoriatarget/map/terrain.bmp";
	BMPHandler::getInstance().Load24bitBMP(provsource, 1, "provinces");
	BMPHandler::getInstance().Load8bitBMP(terrainsource, 1, "terrain");

	Bitmap * terrain = BMPHandler::getInstance().findBitmapByKey("terrain");





	Bitmap * provinces = BMPHandler::getInstance().findBitmapByKey("provinces");
	Bitmap * provincesNew = provinces;
	provincesNew->Buffer = t.landProvinces(5000, 5616, 2160, terrain, provinces);
	BMPHandler::getInstance().SaveBMP(provincesNew, "newprovinces.bmp",5616, 2160, "provinces");

	system("pause");
}