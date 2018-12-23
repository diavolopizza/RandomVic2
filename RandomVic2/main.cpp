#include "utils/BMPHandler.h"
#include "terrain\Terrain.h"
#include "utils\Data.h"
#include "utils\Parser.h"
#include "victoria2/VictoriaModule.h"
#include <memory.h>
#include <random>
#include "utils/Visualizer.h"
/*
Step 1: Generate Terrain/continent shape/rivers
	-continents(check)
		-happy, but add more options for fractal generation
	-climate(check)
	-terrain(acceptable)
		-add westwinds/coastal region
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





//THIS PART FOR ACTUAL VIC2 Generation
Step 3.1: Generate province files for new provinces
	-positions.txt(simple)
		-create candidates for various types of installations, which can afterwards be utilised by the respective generation(simple)
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

*/
int main() {

	//Generation objects
	Data *data = new Data();
	data->getConfig("config.json");
	Parser genericParser;
	Terrain *terrainGenerator = new Terrain(data->random);

	Bitmap *heightMapBMP = new Bitmap(data->width, data->height, 24);
	string heightmapSourceString = data->debugMapFolder + ("heightmap.bmp");
	const char* heightmapsource = heightmapSourceString.c_str();
	heightMapBMP->path = heightmapsource;
	//generate noise map
	if (data->genHeight) {
		heightMapBMP->setBuffer(terrainGenerator->heightMap(heightMapBMP, data->seed, data->fractalFrequency, data->fractalOctaves, data->fractalGain, data->divideThreshold, data->seaLevel, data->complexHeight, data->updateThreshold));

		BMPHandler::getInstance().SaveBMPToFile(heightMapBMP, (data->debugMapFolder + ("heightmap.bmp")).c_str());
	}
	else {
		heightMapBMP = BMPHandler::getInstance().Load24bitBMP(heightmapsource, "heightmap");
		data->height = heightMapBMP->bitmapinfoheader.biHeight;
		data->width = heightMapBMP->bitmapinfoheader.biWidth;
	}
	//create new bmps{
	Bitmap provincesBMP(data->width, data->height, 24);
	Bitmap humidityBMP(data->width, data->height, 24);
	Bitmap continents(data->width, data->height, 24);
	Bitmap regionBMP(data->width, data->height, 24);
	//load 8 bit bitmaps from file to get colourtable
	string simpleTerrainsourceString = data->debugMapFolder + ("simpleterrain.bmp");
	const char* simpleTerrainsource = simpleTerrainsourceString.c_str();
	string terrainsourceString = data->mapSource + ("terrain.bmp");
	const char* terrainsource = terrainsourceString.c_str();
	string riversourceString = data->mapSource + ("rivers.bmp");
	const char* riversource = riversourceString.c_str();
	Bitmap* terrainBMP = BMPHandler::getInstance().Load8bitBMP(terrainsource, "terrain");
	Bitmap* riverBMP = BMPHandler::getInstance().Load8bitBMP(riversource, "rivers");
	//modify size and properties to fit rest of generation
	terrainBMP->setBitmapSize(data->width, data->height);
	riverBMP->setBitmapSize(data->width, data->height);


	if (data->genSimpleTerrain) {
		//create simplistic terrain shape from noise map
		terrainBMP->setBuffer(terrainGenerator->createTerrain(terrainBMP, heightMapBMP->getBuffer(), data->seaLevel, (double)data->landMassPercentage/100.0));
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (data->debugMapFolder + ("simpleterrain.bmp")).c_str());
	}
	else {
		terrainBMP = BMPHandler::getInstance().Load8bitBMP(simpleTerrainsource, "simpleterrain");
	}
	//create provinces
	{
		provincesBMP.setBuffer(terrainGenerator->landProvinces(data->landProv, terrainBMP, &provincesBMP, data->updateThreshold));
		provincesBMP.setBuffer(terrainGenerator->seaProvinces(data->seaProv, data->landProv, terrainBMP, &provincesBMP, data->updateThreshold));
		terrainGenerator->createProvinceMap();
		terrainGenerator->provPixels(&provincesBMP);
		//terrainGenerator->prettyProvinces(&provincesBMP, data->minProvSize);
		terrainGenerator->evaluateCoasts(&provincesBMP);
		//dump to file
		genericParser.writeDefinition((data->debugMapFolder + ("definition.csv")).c_str(), terrainGenerator->provinces);
	}
	//calculate neighbours and dump them
	{
		terrainGenerator->evaluateNeighbours(&provincesBMP);
		genericParser.writeAdjacency((data->debugMapFolder + ("adjacency.csv")).c_str(), terrainGenerator->provinces);
	}
	//assign provinces to regions and dump them
	{
		terrainGenerator->evaluateRegions(data->minProvPerRegion, data->width, data->height);
		terrainGenerator->prettyRegions(&regionBMP);
		genericParser.writeRegions((data->debugMapFolder + ("region.txt")).c_str(), terrainGenerator->regions);
	}
	//assign provinces to continents and dump them
	{
		terrainGenerator->evaluateContinents(data->minProvPerContinent, data->width, data->height);
		terrainGenerator->prettyContinents(&continents);
		genericParser.writeContinents((data->debugMapFolder + ("continent.txt")).c_str(), terrainGenerator->continents);
	}
	if (data->genComplexTerrain) {
		terrainGenerator->humidityMap(heightMapBMP, &humidityBMP, data->seaLevel, data->updateThreshold);
		BMPHandler::getInstance().SaveBMPToFile(&humidityBMP, (data->debugMapFolder + ("humidity.bmp")).c_str());
		//generate terrain and rivers according to simplistic climate model
		terrainGenerator->prettyTerrain(terrainBMP, heightMapBMP, data->seaLevel, data->updateThreshold);
		//generate rivers according to terrain and climate
		terrainGenerator->prettyRivers(riverBMP, heightMapBMP, data->numRivers, data->elevationTolerance, data->seaLevel);
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (data->debugMapFolder + ("terrain.bmp")).c_str());
	}
	else {
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (data->debugMapFolder + ("terrain.bmp")).c_str());
	}
	//Dump all info into map folder
	{
		genericParser.writeClimate((data->debugMapFolder + ("climate.txt")).c_str(), terrainGenerator->provinces);//general
		genericParser.writeDefaultMapHeader((data->debugMapFolder + ("default.map")).c_str(), terrainGenerator->provinces);//general

		printf("Writing Bitmaps to %s folder\n", data->debugMapFolder.c_str());
		//save all the bmps
		BMPHandler::getInstance().SaveBMPToFile(heightMapBMP, (data->debugMapFolder + ("heightmap.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (data->debugMapFolder + ("terrain.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(riverBMP, (data->debugMapFolder + ("rivers.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&continents, (data->debugMapFolder + ("continents.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&regionBMP, (data->debugMapFolder + ("regions.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&provincesBMP, (data->debugMapFolder + ("provinces.bmp")).c_str());
	}

	if (data->genV2)
	{
		VictoriaModule vMod(data, terrainGenerator);
		vMod.dumpMapFiles(data, terrainBMP, riverBMP, &continents, &regionBMP, heightMapBMP, &provincesBMP);
	}

	return 0;
	//errors:
		//deleting small province (prettyprovinces) messes up seastarts
		//simple lakes crash loading textures
		//naval bases
	/*
	Prerequisites:
		-religions for continents

	Step : Generate countries
		-common/countries:
			-color = { 124 40 30 }
			-graphical_culture
			-parties
				-name
				-start/end
				-ideology
				-policies
					-economic
					-trade
					-religious
					.citizenship
					-war
			-optional: unit names
		-history/countries
			-capital province
			-primary_culture
			-religion
			-government
			-plurality
			-nationalvalue
			-literacy
			-civilized
			-prestige
			-political_reforms
			-social reforms
			-ruling party
			-upper_house
			-technologies
			-starting_counciousness
			-unit files(oob)
			-tech schools
		-history/pops/1836.1.1
			-for each region
				-each province
		-history/provinces
		-history/units
		-Flags

	Step 2: Distribute provinces to countries






	*/


	//system("pause");
}