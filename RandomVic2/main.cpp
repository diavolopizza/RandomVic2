#include "utils/BMPHandler.h"
#include "terrain\Terrain.h"
#include "utils\Data.h"
#include "utils\Parser.h"
#include "victoria2/VictoriaModule.h"
#include "utils/CountryGenerator.h"
#include <memory.h>
#include <random>
#include "utils/Visualizer.h"
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
	Data::getInstance();// new Data();
	Data::getInstance().getConfig("config.json");
	Parser genericParser;
	Terrain *terrainGenerator = new Terrain(Data::getInstance().random);

	Bitmap *heightMapBMP = new Bitmap(Data::getInstance().width, Data::getInstance().height, 24);
	string heightmapSourceString = Data::getInstance().debugMapFolder + ("heightmap.bmp");
	const char* heightmapsource = heightmapSourceString.c_str();
	//generate noise map
	if (Data::getInstance().genHeight) {
		heightMapBMP->setBuffer(terrainGenerator->heightMap(heightMapBMP, Data::getInstance().seed, Data::getInstance().fractalFrequency, Data::getInstance().fractalOctaves, Data::getInstance().fractalGain, Data::getInstance().divideThreshold, Data::getInstance().seaLevel, Data::getInstance().complexHeight, Data::getInstance().updateThreshold));

		BMPHandler::getInstance().SaveBMPToFile(heightMapBMP, (Data::getInstance().debugMapFolder + ("heightmap.bmp")).c_str());
	}
	else {
		heightMapBMP = BMPHandler::getInstance().Load24bitBMP(heightmapsource, "heightmap");
		Data::getInstance().height = heightMapBMP->bInfoHeader.biHeight;
		Data::getInstance().width = heightMapBMP->bInfoHeader.biWidth;
	}
	//create new bmps{
	Bitmap provincesBMP(Data::getInstance().width, Data::getInstance().height, 24);
	Bitmap humidityBMP(Data::getInstance().width, Data::getInstance().height, 24);
	Bitmap continents(Data::getInstance().width, Data::getInstance().height, 24);
	Bitmap regionBMP(Data::getInstance().width, Data::getInstance().height, 24);
	//load 8 bit bitmaps from file to get colourtable
	string simpleTerrainsourceString = Data::getInstance().debugMapFolder + ("simpleterrain.bmp");
	const char* simpleTerrainsource = simpleTerrainsourceString.c_str();
	string terrainsourceString = Data::getInstance().mapSource + ("terrain.bmp");
	const char* terrainsource = terrainsourceString.c_str();
	string riversourceString = Data::getInstance().mapSource + ("rivers.bmp");
	const char* riversource = riversourceString.c_str();
	Bitmap* terrainBMP = BMPHandler::getInstance().Load8bitBMP(terrainsource, "terrain");
	Bitmap* riverBMP = BMPHandler::getInstance().Load8bitBMP(riversource, "rivers");
	//modify size and properties to fit rest of generation
	terrainBMP->setBitmapSize(Data::getInstance().width, Data::getInstance().height);
	riverBMP->setBitmapSize(Data::getInstance().width, Data::getInstance().height);


	if (Data::getInstance().genSimpleTerrain) {
		//create simplistic terrain shape from noise map
		terrainGenerator->createTerrain(terrainBMP, heightMapBMP, Data::getInstance().seaLevel, (double)Data::getInstance().landMassPercentage/100.0);
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (Data::getInstance().debugMapFolder + ("simpleterrain.bmp")).c_str());
		//generate rivers according to terrain and climate
		terrainGenerator->prettyRivers(riverBMP, heightMapBMP, Data::getInstance().numRivers, Data::getInstance().elevationTolerance, Data::getInstance().seaLevel);
	}
	else {
		terrainBMP = BMPHandler::getInstance().Load8bitBMP(simpleTerrainsource, "simpleterrain");
	}
	//create provinces
	{
		provincesBMP.setBuffer(terrainGenerator->landProvinces(Data::getInstance().landProv, terrainBMP, &provincesBMP, riverBMP, Data::getInstance().updateThreshold));
		BMPHandler::getInstance().SaveBMPToFile(&provincesBMP, (Data::getInstance().debugMapFolder + ("provinces.bmp")).c_str());
		provincesBMP.setBuffer(terrainGenerator->seaProvinces(Data::getInstance().seaProv, Data::getInstance().landProv, terrainBMP, &provincesBMP, riverBMP, Data::getInstance().updateThreshold));
		terrainGenerator->createProvinceMap();
		terrainGenerator->provPixels(&provincesBMP);
		terrainGenerator->prettyProvinces(&provincesBMP, riverBMP, Data::getInstance().minProvSize);
		terrainGenerator->evaluateCoasts(&provincesBMP);
		//dump to file
		genericParser.writeDefinition((Data::getInstance().debugMapFolder + ("definition.csv")).c_str(), terrainGenerator->provinces);
	}
	//calculate neighbours and dump them
	{
		terrainGenerator->evaluateNeighbours(&provincesBMP);
		genericParser.writeAdjacency((Data::getInstance().debugMapFolder + ("adjacency.csv")).c_str(), terrainGenerator->provinces);
	}
	//assign provinces to regions and dump them
	{
		terrainGenerator->evaluateRegions(Data::getInstance().minProvPerRegion, Data::getInstance().width, Data::getInstance().height);
		terrainGenerator->prettyRegions(&regionBMP);
		genericParser.writeRegions((Data::getInstance().debugMapFolder + ("region.txt")).c_str(), terrainGenerator->regions);
	}
	//assign provinces to continents and dump them
	{
		terrainGenerator->evaluateContinents(Data::getInstance().minProvPerContinent, Data::getInstance().width, Data::getInstance().height);
		terrainGenerator->prettyContinents(&continents);
		genericParser.writeContinents((Data::getInstance().debugMapFolder + ("continent.txt")).c_str(), terrainGenerator->continents);
	}
	if (Data::getInstance().genComplexTerrain) {
		terrainGenerator->humidityMap(heightMapBMP, &humidityBMP, Data::getInstance().seaLevel, Data::getInstance().updateThreshold);
		BMPHandler::getInstance().SaveBMPToFile(&humidityBMP, (Data::getInstance().debugMapFolder + ("humidity.bmp")).c_str());
		//generate terrain and rivers according to simplistic climate model
		terrainGenerator->prettyTerrain(terrainBMP, heightMapBMP, Data::getInstance().seaLevel, Data::getInstance().updateThreshold);
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (Data::getInstance().debugMapFolder + ("terrain.bmp")).c_str());
	}
	else {
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (Data::getInstance().debugMapFolder + ("terrain.bmp")).c_str());
	}
	//Dump all info into map folder
	{
		genericParser.writeClimate((Data::getInstance().debugMapFolder + ("climate.txt")).c_str(), terrainGenerator->provinces);//general
		genericParser.writeDefaultMapHeader((Data::getInstance().debugMapFolder + ("default.map")).c_str(), terrainGenerator->provinces);//general

		printf("Writing Bitmaps to %s folder\n", Data::getInstance().debugMapFolder.c_str());
		//save all the bmps
		BMPHandler::getInstance().SaveBMPToFile(heightMapBMP, (Data::getInstance().debugMapFolder + ("heightmap.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (Data::getInstance().debugMapFolder + ("terrain.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(riverBMP, (Data::getInstance().debugMapFolder + ("rivers.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&continents, (Data::getInstance().debugMapFolder + ("continents.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&regionBMP, (Data::getInstance().debugMapFolder + ("regions.bmp")).c_str());
		BMPHandler::getInstance().SaveBMPToFile(&provincesBMP, (Data::getInstance().debugMapFolder + ("provinces.bmp")).c_str());
	}

	CountryGenerator cG(Data::getInstance().random);
	cG.distributeCountries(100, 0, terrainGenerator->regions);
	Bitmap countryBMP(Data::getInstance().width, Data::getInstance().height, 24);
	cG.countryBMP(&countryBMP);
	BMPHandler::getInstance().SaveBMPToFile(&countryBMP, (Data::getInstance().debugMapFolder + ("countries.bmp")).c_str());


	if (Data::getInstance().genV2)
	{
		VictoriaModule vMod(&Data::getInstance(), terrainGenerator);
		vMod.dumpMapFiles(&Data::getInstance(), terrainBMP, riverBMP, &continents, &regionBMP, heightMapBMP, &provincesBMP);
	}
	terrainGenerator->sanityChecks(&provincesBMP);

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