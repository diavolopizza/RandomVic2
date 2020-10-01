#define D_SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#pragma warning(disable:4996) 
#include "utils/BMPHandler.h"
#include "terrain\Terrain.h"
#include "utils\Data.h"
#include "utils\Parser.h"
#include "victoria2/VictoriaModule.h"
#include "utils/CountryGenerator.h"
#include <memory.h>
#include <random>
#include "utils/Visualizer.h"
#include "utils/MapMerger.h"
/*
Step 1: Generate Terrain/continent shape/rivers
	-continents(check)
		-happy, but add more options for fractal generation
	-climate(check)
	-humidity/precipitation
	-terrain(acceptable)
		-add precipitation into calculation
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
	//Data data =
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	Data::getInstance();
	Data::getInstance().getConfig("C:/Users/Paul/Documents/Visual Studio 2017/Projects/RandomVic2/config.json");
	Parser genericParser;
	Terrain terrainGenerator;
	//const Bitmap heightMapBMP;
	const string heightmapSourceString = Data::getInstance().debugMapsPath + ("heightmap.bmp");
	const char* heightmapsource = heightmapSourceString.c_str();
	uint32_t layer = 0;
	//generate noise map
	Bitmap heightMapBMP;
	if (Data::getInstance().genHeight)
	{
		heightMapBMP = Bitmap(Data::getInstance().width, Data::getInstance().height, 24, terrainGenerator.heightMap(Data::getInstance().seed));
	}
	else {
		heightMapBMP = Bitmap(Data::getInstance().width, Data::getInstance().height, 24, BMPHandler::getInstance().Load24bitBMP(heightmapsource, "heightmap"));
	}
	//(Data::getInstance().width, Data::getInstance().height, 24, Data::getInstance().genHeight ?
	//	terrainGenerator.heightMap(Data::getInstance().seed) : loadedHeightMap.getBuffer());
	Data::getInstance().height = heightMapBMP.bInfoHeader.biHeight;
	Data::getInstance().width = heightMapBMP.bInfoHeader.biWidth;
	BMPHandler::getInstance().SaveBMPToFile(heightMapBMP, (Data::getInstance().debugMapsPath + ("heightmap.bmp")).c_str());
	Bitmap provincesBMP(Data::getInstance().width, Data::getInstance().height, 24);
	Bitmap humidityBMP(Data::getInstance().width, Data::getInstance().height, 24);
	Bitmap continents(Data::getInstance().width, Data::getInstance().height, 24);
	Bitmap regionBMP(Data::getInstance().width, Data::getInstance().height, 24);
	//load 8 bit bitmaps from file to get colourtable
	string simpleTerrainsourceString = Data::getInstance().debugMapsPath + ("simpleterrain.bmp");
	const char* simpleTerrainsource = simpleTerrainsourceString.c_str();
	string terrainsourceString = Data::getInstance().mapSource + ("terrain.bmp");
	const char* terrainsource = terrainsourceString.c_str();
	string riversourceString = Data::getInstance().mapSource + ("rivers.bmp");
	const char* riversource = riversourceString.c_str();
	Bitmap terrainBMP = BMPHandler::getInstance().Load8bitBMP(terrainsource, "terrain");
	Bitmap riverBMP = BMPHandler::getInstance().Load8bitBMP(riversource, "rivers");
	//modify size and properties to fit rest of generation
	terrainBMP.setBitmapSize(Data::getInstance().width, Data::getInstance().height);
	riverBMP.setBitmapSize(Data::getInstance().width, Data::getInstance().height);


	if (Data::getInstance().genSimpleTerrain) {
		//create simplistic terrain shape from noise map
		terrainGenerator.createTerrain(&terrainBMP, heightMapBMP);
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (Data::getInstance().debugMapsPath + ("simpleterrain.bmp")).c_str());
		//generate rivers according to terrain and climate
		terrainGenerator.generateRivers(&riverBMP, heightMapBMP);
		terrainGenerator.prettyRivers(&riverBMP, heightMapBMP);
	}
	else {
		terrainBMP = BMPHandler::getInstance().Load8bitBMP(simpleTerrainsource, "simpleterrain");
	}
	//return 0;
	//create provinces
	{
		provincesBMP.setBuffer(terrainGenerator.landProvinces(Data::getInstance().landProv, terrainBMP, &provincesBMP, riverBMP, Data::getInstance().updateThreshold));
		BMPHandler::getInstance().SaveBMPToFile(provincesBMP, (Data::getInstance().debugMapsPath + ("provinces.bmp")).c_str());
		provincesBMP.setBuffer(terrainGenerator.seaProvinces(Data::getInstance().seaProv, Data::getInstance().landProv, terrainBMP, &provincesBMP, riverBMP, Data::getInstance().updateThreshold));
		BMPHandler::getInstance().SaveBMPToFile(provincesBMP, (Data::getInstance().debugMapsPath + ("provinces.bmp")).c_str());
		terrainGenerator.createProvinceMap();
		terrainGenerator.provPixels(&provincesBMP);
		terrainGenerator.prettyProvinces(&provincesBMP, riverBMP, Data::getInstance().minProvSize);
		{
			terrainGenerator.evaluateNeighbours(provincesBMP);
			genericParser.writeAdjacency((Data::getInstance().debugMapsPath + ("adjacency.csv")).c_str(), terrainGenerator.provinces);
		}
		terrainGenerator.evaluateCoasts(provincesBMP);
		//dump to file
		genericParser.writeDefinition((Data::getInstance().debugMapsPath + ("definition.csv")).c_str(), terrainGenerator.provinces);
	}
	//calculate neighbours and dump them

	//assign provinces to regions and dump them
	{
		terrainGenerator.evaluateRegions(Data::getInstance().minProvPerRegion, Data::getInstance().width, Data::getInstance().height);
		terrainGenerator.prettyRegions(&regionBMP);
		genericParser.writeRegions((Data::getInstance().debugMapsPath + ("region.txt")).c_str(), terrainGenerator.regions);
	}
	//assign provinces to continents and dump them
	{
		terrainGenerator.evaluateContinents(Data::getInstance().minProvPerContinent, Data::getInstance().width, Data::getInstance().height);
		terrainGenerator.prettyContinents(&continents);
		genericParser.writeContinents((Data::getInstance().debugMapsPath + ("continent.txt")).c_str(), terrainGenerator.continents);
	}
	if (Data::getInstance().genComplexTerrain) {
		//terrainGenerator->humidityMap(heightMapBMP, &humidityBMP, Data::getInstance().seaLevel, Data::getInstance().updateThreshold);
		//BMPHandler::getInstance().SaveBMPToFile(&humidityBMP, (Data::getInstance().debugMapsPath + ("humidity.bmp")).c_str());
		//generate terrain and rivers according to simplistic climate model
		terrainGenerator.prettyTerrain(&terrainBMP, heightMapBMP, Data::getInstance().seaLevel, Data::getInstance().updateThreshold);
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (Data::getInstance().debugMapsPath + ("terrain.bmp")).c_str());
	}
	else {
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (Data::getInstance().debugMapsPath + ("terrain.bmp")).c_str());
	}
	//Dump all info into map folder
	{
		genericParser.writeClimate((Data::getInstance().debugMapsPath + ("climate.txt")).c_str(), terrainGenerator.provinces);//general
		genericParser.writeDefaultMapHeader((Data::getInstance().debugMapsPath + ("default.map")).c_str(), terrainGenerator.provinces);//general

		printf("Writing Bitmaps to %s folder\n", Data::getInstance().debugMapsPath.c_str());
		//save all the bmps
		BMPHandler::getInstance().SaveBMPToFile(heightMapBMP, (Data::getInstance().debugMapsPath + ("heightmap.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (Data::getInstance().debugMapsPath + ("terrain.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(riverBMP, (Data::getInstance().debugMapsPath + ("rivers.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(continents, (Data::getInstance().debugMapsPath + ("continents.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(regionBMP, (Data::getInstance().debugMapsPath + ("regions.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(provincesBMP, (Data::getInstance().debugMapsPath + ("provinces.bmp")).c_str());
	}

	CountryGenerator cG(&terrainGenerator);
	cG.distributeCountries(Data::getInstance().maxNumOfCountries, 0, terrainGenerator.regions);
	cG.determineDimensions();
	cG.sanityChecks();
	Bitmap countryBMP = cG.countryBMP();
	BMPHandler::getInstance().SaveBMPToFile(countryBMP, (Data::getInstance().debugMapsPath + ("countries.bmp")).c_str());
	Bitmap developmentBMP = cG.civilizationBMP();
	BMPHandler::getInstance().SaveBMPToFile(developmentBMP, (Data::getInstance().debugMapsPath + ("development.bmp")).c_str());


	if (Data::getInstance().genV2)
	{
		//	VictoriaModule vMod(&Data::getInstance(), terrainGenerator);
		//	vMod.dumpMapFiles(&Data::getInstance(), terrainBMP, riverBMP, &continents, &regionBMP, heightMapBMP, &provincesBMP);
	}
	terrainGenerator.sanityChecks(provincesBMP);
	MapMerger merger;
	Bitmap heightRiver = merger.mergeHeightRiver(heightMapBMP, riverBMP);
	BMPHandler::getInstance().SaveBMPToFile(heightRiver, (Data::getInstance().debugMapsPath + ("heightRiver.bmp")).c_str());
	Bitmap countryRiver = merger.mergeCountryRiverProvince(countryBMP, riverBMP, provincesBMP);
	BMPHandler::getInstance().SaveBMPToFile(countryRiver, (Data::getInstance().debugMapsPath + ("countryRiver.bmp")).c_str());

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
	system("pause");
	return 0;
	//errors:
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