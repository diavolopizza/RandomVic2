#include "utils/BMPHandler.h"
#include "terrain\Terrain.h"
#include "utils\Data.h"
#include <random>

/*
Step 1: Generate Terrain/continent shape/rivers
Step 2: Generate Provinces on new terrain
			-provinces.bmp
Step 2.2: Generate new regions for new provinces
			-region.txt
			-continent.txt
				-load provinces before and then get their respective numbers, location and then check on what continent they are
				or
				-generate continents from regions
					-but how...
			-climate.txt
Step 3.1: Read general info from bmps, such as provinces, where etc
Step 3.1: Generate province files for new provinces
	-positions.txt
	-definition.csv
	-default.map number of provinces and seastarts(b=10 land, b=255 sea)
	-adjacencies.csv

Step 4.1: Output scenario info
			-create province txts in history/provinces




Components:
	-BMPHandler/Bitmap
	-Entities
		-Province
		-State
			-naval bases
		-Country
			-everything for new country
				-Flags
				-politics
		-
	-Terraingenerator
		-Provincegenerator
			-province.bmp
			-
	-Country distribution
	


*/

using namespace std;

int main() {
	Data *data = new Data();
	Terrain t(data->random);
	cout << data->VicPath;
	cout << data->VicPath + "map/provinces.bmp";
	string provsourceString = data->VicPath + ("map/provinces.bmp");
	string terrainsourceString = data->VicPath + ("map/terrain.bmp");
	const char* provsource = provsourceString.c_str();
	const char* terrainsource = terrainsourceString.c_str();
	Bitmap * provinces = BMPHandler::getInstance().Load24bitBMP(provsource, "provinces");
	Bitmap * terrain = BMPHandler::getInstance().Load8bitBMP(terrainsource, "terrain");

	Bitmap * provincesNew = provinces;
	provincesNew->Buffer = t.landProvinces(5000, terrain, provinces);
	BMPHandler::getInstance().SaveBMPToFile(provincesNew, (data->modPath.append("map/provinces.bmp")).c_str());

	system("pause");
}