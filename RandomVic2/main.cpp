#include "utils/BMPHandler.h"
#include "terrain\Terrain.h"
#include "utils\Data.h"
#include "utils\Parser.h"
#include "CountryCreation.h"
#include "Victoria2Parser.h"
#include <memory.h>
#include <random>
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
	//Params
	uint32_t width = 5616;
	uint32_t height = 2160;
	uint32_t landProv = 2000;
	uint32_t seaProv = 3000;
	uint32_t minProvPerContinent = 1;
	uint32_t minProvPerRegion = 5;
	uint32_t seed = 2;
	//Generation objects
	Data *data = new Data(seed);
	Parser genericParser;
	Terrain terrainGenerator(data->random, width, height);

	//create new bmps{
	Bitmap provincesBMP(width, height, 24);
	Bitmap heightMapBMP(width, height, 24);
	Bitmap continents(width, height, 24);
	Bitmap regionBMP(width, height, 24);
	//load 8 bit bitmaps from file to get colourtable
	string terrainsourceString = data->mapSource + ("terrain.bmp");
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
	float fractalGain = 0.3298;
	uint32_t borderLimiter = 10;
	uint32_t minProvSize = 100;
	uint32_t elevationTolerance = 5;
	uint32_t riverAmount = 0;
	uint32_t seaLevel = 139;

	//generate noise map
	heightMapBMP.setBuffer(terrainGenerator.heightMap(&heightMapBMP, seed, fractalFrequency, fractalOctaves,fractalGain, borderLimiter, seaLevel));
	//create simplistic terrain shape from noise map
	terrainBMP->setBuffer(terrainGenerator.createTerrain(terrainBMP, heightMapBMP.getBuffer(), seaLevel));

	//create provinces
	{
		provincesBMP.setBuffer( terrainGenerator.landProvinces(landProv, terrainBMP, &provincesBMP));
		provincesBMP.setBuffer(terrainGenerator.seaProvinces(seaProv, landProv, terrainBMP, &provincesBMP));
		terrainGenerator.createProvinceMap();
		terrainGenerator.provPixels(&provincesBMP);
		//terrainGenerator.prettyProvinces(&provincesBMP, minProvSize);
		//terrainGenerator.evaluateCoasts(&provincesBMP);
		//dump to file
		genericParser.writeDefinition((data->mapPath + ("definition.csv")).c_str(), terrainGenerator.provinces);
	}
	//calculate neighbours and dump them
	{
		terrainGenerator.evaluateNeighbours(&provincesBMP);
		genericParser.writeAdjacency((data->mapPath + ("adjacency.csv")).c_str(), terrainGenerator.provinces);
	}
	//assign provinces to continents and dump them
	{
		terrainGenerator.evaluateContinents(minProvPerContinent);
		terrainGenerator.prettyContinents(&continents);
		genericParser.writeContinents((data->mapPath + ("continent.txt")).c_str(), terrainGenerator.continents);
	}
	//assign provinces to regions and dump them
	{
		terrainGenerator.evaluateRegions(minProvPerRegion);
		terrainGenerator.prettyRegions(&regionBMP);
		genericParser.writeRegions((data->mapPath + ("region.txt")).c_str(), terrainGenerator.regions);
	}
	//generate terrain and rivers according to simplistic climate model
	{
		//terrainGenerator.prettyTerrain(terrainBMP, &heightMapBMP, seaLevel);
		//generate rivers according to terrain and climate
		terrainGenerator.prettyRivers(riverBMP, &heightMapBMP, riverAmount,elevationTolerance, seaLevel);
	}
	//Dump all info into map folder
	{
		genericParser.writeClimate((data->mapPath + ("climate.txt")).c_str(), terrainGenerator.provinces);//general
		genericParser.writeDefaultMapHeader((data->mapPath + ("default.map")).c_str(), terrainGenerator.provinces);//general

		printf("Writing Bitmaps to %s folder\n", data->mapPath.c_str());
		//save all the bmps
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (data->mapPath + ("terrain.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(riverBMP, (data->mapPath + ("rivers.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&continents, (data->mapPath + ("continents.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&regionBMP, (data->mapPath + ("regions.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&heightMapBMP, (data->mapPath + ("heightmap.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&provincesBMP, (data->mapPath + ("provinces.bmp")).c_str());
	}
	//VIC2 stuff starts here	
	Victoria2Parser::createFolders(data->modPath);
	//Dump all info into map folder
	{
		genericParser.writeDefinition((data->modPath + ("map/definition.csv")).c_str(), terrainGenerator.provinces);
		genericParser.writeAdjacency((data->modPath + ("map/adjacency.csv")).c_str(), terrainGenerator.provinces);
		genericParser.writeContinents((data->modPath + ("map/continent.txt")).c_str(), terrainGenerator.continents);
		genericParser.writeRegions((data->modPath + ("map/region.txt")).c_str(), terrainGenerator.regions);
		genericParser.writeClimate((data->modPath + ("map/climate.txt")).c_str(), terrainGenerator.provinces);//general
		genericParser.writeDefaultMapHeader((data->modPath + ("map/default.map")).c_str(), terrainGenerator.provinces);//general

		printf("Writing Bitmaps to %s folder\n", data->modPath.c_str());
		//save all the bmps
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (data->modPath + ("map/terrain.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(riverBMP, (data->modPath + ("map/rivers.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&continents, (data->modPath + ("map/continents.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&regionBMP, (data->modPath + ("map/regions.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&heightMapBMP, (data->modPath + ("map/heightmap.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&provincesBMP, (data->modPath + ("map/provinces.bmp")).c_str());
	}
	CountryCreation::distributeCountries(terrainGenerator.provinces);
	Victoria2Parser::writeCountries(data->modPath, terrainGenerator.provinces);
	Victoria2Parser::writePops(data->modPath, terrainGenerator.provinces);
	Victoria2Parser::writeClimate(data->modPath, (data->VicPath + ("map/climate.txt")).c_str() , terrainGenerator.provinces);
	Victoria2Parser::writeDefaultMapHeader(data->modPath, (data->VicPath + ("map/default.map")).c_str(), terrainGenerator.provinces);
	Victoria2Parser::writePositions(data->modPath, terrainGenerator.provinces);
	Victoria2Parser::writeAdjacencies(data->modPath);

	//errors:
		//climate.txt misses header
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