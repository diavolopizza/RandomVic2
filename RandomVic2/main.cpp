#include "utils/BMPHandler.h"
#include "terrain\Terrain.h"
#include "utils\Data.h"
#include "utils\Parser.h"
#include <memory.h>
#include <random>
/*
Step 1: Generate Terrain/continent shape/rivers
	-continents(check)
	-climate(check)
	-terrain(acceptable)
	-rivers(good)
Step 2: Generate Provinces on new terrain
			-provinces.bmp (check)
Step 2.2: Generate new regions for new provinces
			-continent.txt(check)
			-region.txt(check)
			-climate.txt(check)
Step 2.3: Generate colourmaps
			-colormap_political.dds
			-colormap_water.dds
			-colormap.dds
Step 3.1: Generate province files for new provinces
			-positions.txt
				-create candidates for various types of installations, which can afterwards be utilised by the respective generation
			-definition.csv(check)
			-default.map (check)
			-number of provinces and seastarts(b=10 land, b=255 sea)
			-adjacencies.csv(check)
//THIS PART FOR ACTUAL VIC2 Generation
Step 3.1: Read general info from map folder
			-default.map (check)
			-number of provinces and seastarts(b=10 land, b=255 sea)
			-definition.csv(check)
			-positions.txt


Step 4.1: Scenario Info:
	-create Countries/Customize old ones
		-politics, progress etc
		-assign provinces to countries
Step 4.1: Output scenario info
			-create province txts in history/provinces

*/
int main() {
	//Params
	uint32_t width = 5616;
	uint32_t height = 2160;
	uint32_t landProv = 3000;
	uint32_t seaProv = 5000;
	uint32_t minProvPerContinent = 100;
	//Generation objects
	Data *data = new Data();
	Parser P;
	Terrain t(data->random, width, height);

	//create new bmps{
	Bitmap provincesBMP(width, height, 24);
	Bitmap heightMapBMP(width, height, 24);
	Bitmap continents(width, height, 24);
	Bitmap regionBMP(width, height, 24);
	//load 8 bit bitmaps from file to get colourtable
	string terrainsourceString = data->mapPath + ("terrain.bmp");
	const char* terrainsource = terrainsourceString.c_str();
	string riversourceString = data->mapPath + ("rivers.bmp");
	const char* riversource = riversourceString.c_str();
	Bitmap* terrainBMP = BMPHandler::getInstance().Load8bitBMP(terrainsource, "terrain");
	Bitmap* riverBMP = BMPHandler::getInstance().Load8bitBMP(riversource, "rivers");
	//modify size and properties to fit rest of generation
	terrainBMP->setBitmapSize(width, height);
	riverBMP->setBitmapSize(width, height);
	
	float fractalFrequency = 0.0006f;
	uint32_t fractalOctaves = 11;
	float fractalGain = 0.5;
	uint32_t borderLimiter = 10;
	uint32_t minProvSize = 30;
	uint32_t elevationTolerance = 5;
	uint32_t riverAmount = 1000;

	//generate noise map
	heightMapBMP.Buffer = t.heightMap(&heightMapBMP, 3, fractalFrequency, fractalOctaves,fractalGain, borderLimiter);
	//create simplistic terrain shape from noise map
	terrainBMP->Buffer = t.createTerrain(terrainBMP, heightMapBMP.Buffer);

	//create provinces
	{
		provincesBMP.Buffer = t.landProvinces(landProv, terrainBMP, &provincesBMP);
		provincesBMP.Buffer = t.seaProvinces(seaProv, landProv, terrainBMP, &provincesBMP);
		t.createProvinceMap();
		t.provPixels(&provincesBMP);
		t.prettyProvinces(&provincesBMP, minProvSize);
		t.evaluateCoasts(&provincesBMP);
		//dump to file
		P.writeDefinition((data->mapPath + ("definition.csv")).c_str(), t.provinces);
	}
	//calculate neighbours and dump them
	{
		t.evaluateNeighbours(&provincesBMP);
		P.writeAdjacency((data->mapPath + ("adjacency.csv")).c_str(), t.provinces);
	}
	//assign provinces to continents and dump them
	{
		t.evaluateContinents(minProvPerContinent);
		t.prettyContinents(&continents);
		P.writeContinents((data->mapPath + ("continent.txt")).c_str(), t.continents);
	}
	//assign provinces to regions and dump them
	{
		t.evaluateRegions();
		t.prettyRegions(&regionBMP);
		P.writeRegions((data->mapPath + ("region.txt")).c_str(), t.regions);
	}
	//generate terrain and rivers according to simplistic climate model
	{
		t.prettyTerrain(terrainBMP, &heightMapBMP);
		//generate rivers according to terrain and climate
		t.prettyRivers(riverBMP, &heightMapBMP, riverAmount,elevationTolerance);
	}

	//Dump all info into map folder
	{
		P.writeClimate((data->mapPath + ("climate.txt")).c_str(), t.provinces);//general
		P.writeDefaultMapHeader((data->mapPath + ("default.map")).c_str(), t.provinces);//general

		printf("Writing Bitmaps to %s folder\n", data->mapPath.c_str());
		//save all the bmps
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (data->mapPath + ("terrain.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(riverBMP, (data->mapPath + ("rivers.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&continents, (data->mapPath + ("continents.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&regionBMP, (data->mapPath + ("regions.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&heightMapBMP, (data->mapPath + ("heightmap.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&provincesBMP, (data->mapPath + ("provinces.bmp")).c_str());
	}
	system("pause");
}