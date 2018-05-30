#include "utils/BMPHandler.h"
#include "terrain\Terrain.h"
#include "utils\Data.h"
#include "utils\Parser.h"
#include <memory.h>
#include <random>
#define _CRT_SECURE_NO_WARNINGS
/*
Step 1: Generate Terrain/continent shape/rivers
	-continents(check)
	-climate(check)
	-terrain(acceptable)
	-rivers
Step 2: Generate Provinces on new terrain
			-provinces.bmp (check)
Step 2.2: Generate new regions for new provinces
			-continent.txt(check)
			-region.txt(check)
			-climate.txt(check)

//THIS PART FOR ACTUAL VIC2 Generation
Step 3.1: Read general info from bmps, such as provinces, where etc
Step 3.1: Generate province files for new provinces
	-positions.txt
	-definition.csv(check)
	-default.map (check)
		-number of provinces and seastarts(b=10 land, b=255 sea)
	-adjacencies.csv(check)

Step 4.1: Scenario Info:
	-create Countries/Customize old ones
		-politics, progress etc
		-assign provinces to countries
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
int main() {
	Data *data = new Data();
	Parser P;
	uint32_t width = 2048;
	uint32_t height = 2048;
	uint32_t landProv = 8000;
	uint32_t seaProv = 10000;
	Terrain t(data->random, width, height);
	string terrainsourceString = data->VicPath + ("map/terrain.bmp");
	const char* terrainsource = terrainsourceString.c_str();
	//create all bmps
	Bitmap provincesBMP(width, height, 24);
	Bitmap* terrainBMP = BMPHandler::getInstance().Load8bitBMP(terrainsource, "terrain");
	Bitmap heightMapBMP(width, height, 24);
	Bitmap continents(width, height, 24);
	Bitmap regionBMP(width, height, 24);
	Bitmap riverBMP(width, height, 24);
	terrainBMP->setBitmapSize(width, height);

	continents.setBitmapSize(width, height);
	regionBMP.setBitmapSize(width, height);
	provincesBMP.setBitmapSize(width, height);

	heightMapBMP.Buffer = t.heightMap(&heightMapBMP, 3);
	terrainBMP->Buffer = t.createTerrain(terrainBMP, heightMapBMP.Buffer);

	//create provinces
	provincesBMP.Buffer = t.landProvinces(landProv, terrainBMP, &provincesBMP);
	provincesBMP.Buffer = t.seaProvinces(seaProv, landProv, terrainBMP, &provincesBMP);
	P.writeDefinition((data->modPath + ("map/definition.csv")).c_str(), t.provinces);
	t.createProvinceMap();

	//generate continent bmp
	t.provPixels(&provincesBMP);
	t.evaluateCoasts(&provincesBMP);
	t.prettyProvinces(&provincesBMP);
	t.evaluateNeighbours(&provincesBMP);
	cout << "writeAdjacency" << endl;;
	P.writeAdjacency((data->modPath + ("map/adjacency.csv")).c_str(), t.provinces);
	t.evaluateContinents();
	t.prettyContinents(&continents);
	P.writeContinents((data->modPath + ("map/continent.txt")).c_str(), t.continents);
	t.evaluateRegions();
	t.prettyRegions(&regionBMP);
	t.prettyTerrain(terrainBMP, &heightMapBMP);
	t.prettyRivers(&riverBMP, &heightMapBMP);

	//Dump all info into map files
	P.writeRegions((data->modPath + ("map/region.txt")).c_str(), t.regions);//general
	P.writeClimate((data->modPath + ("map/climate.txt")).c_str(), t.provinces);//general
	P.writeDefaultMapHeader((data->modPath + ("map/default.map")).c_str(), t.provinces);//general
	cout << "WRITE BMPS" << endl;
	//save all the bmps
	BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (data->modPath + ("map/terrain.bmp")).c_str());
	BMPHandler::getInstance().SaveBMPToFile(&continents, (data->modPath + ("map/continents.bmp")).c_str());
	BMPHandler::getInstance().SaveBMPToFile(&regionBMP, (data->modPath + ("map/regions.bmp")).c_str());
	BMPHandler::getInstance().SaveBMPToFile(&heightMapBMP, (data->modPath + ("map/heightmap.bmp")).c_str());
	BMPHandler::getInstance().SaveBMPToFile(&provincesBMP, (data->modPath + ("map/provinces.bmp")).c_str());

}