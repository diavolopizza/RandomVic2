#include "VictoriaModule.h"



VictoriaModule::VictoriaModule(Data *data, ProvinceGenerator *terrainGenerator)
{
	Parser genericParser;
	//VIC2 stuff starts here
	Victoria2Parser::createFolders(data->modPath);
	//Dump all info into map folder
	{
		genericParser.writeDefinition((data->modPath + ("map/definition.csv")).c_str(), terrainGenerator->provinces);
		genericParser.writeAdjacency((data->modPath + ("map/adjacency.csv")).c_str(), terrainGenerator->provinces);
		genericParser.writeContinents((data->modPath + ("map/continent.txt")).c_str(), terrainGenerator->continents);
		genericParser.writeRegions((data->modPath + ("map/region.txt")).c_str(), terrainGenerator->regions);
		genericParser.writeClimate((data->modPath + ("map/climate.txt")).c_str(), terrainGenerator->provinces);//general
		genericParser.writeDefaultMapHeader((data->modPath + ("map/default.map")).c_str(), terrainGenerator->provinces);//general
	}
	CountryCreation::distributeCountries(terrainGenerator->provinces);
	Victoria2Parser::writeCountries(data->modPath, terrainGenerator->provinces);
	Victoria2Parser::writePops(data->modPath, terrainGenerator->provinces);
	Victoria2Parser::writeClimate(data->modPath, (data->mapSource + ("/climate.txt")).c_str(), terrainGenerator->provinces);
	Victoria2Parser::writeDefaultMapHeader(data->modPath, (data->mapSource + ("/default.map")).c_str(), terrainGenerator->provinces);
	Victoria2Parser::writePositions(data->modPath, terrainGenerator->provinces);
	Victoria2Parser::writeAdjacencies(data->modPath);
}


VictoriaModule::~VictoriaModule()
{
}

void VictoriaModule::dumpMapFiles(Data* data, Bitmap terrainBMP, Bitmap riverBMP, Bitmap continents, Bitmap regionBMP, Bitmap heightMapBMP, Bitmap provincesBMP)
{
	printf("Writing Bitmaps to %s folder\n", data->modPath.c_str());
	//save all the bmps
	BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (data->modPath + ("map/terrain.bmp")).c_str());
	BMPHandler::getInstance().SaveBMPToFile(riverBMP, (data->modPath + ("map/rivers.bmp")).c_str());
	BMPHandler::getInstance().SaveBMPToFile(continents, (data->modPath + ("map/continents.bmp")).c_str());
	BMPHandler::getInstance().SaveBMPToFile(regionBMP, (data->modPath + ("map/regions.bmp")).c_str());
	BMPHandler::getInstance().SaveBMPToFile(heightMapBMP, (data->modPath + ("map/heightmap.bmp")).c_str());
	BMPHandler::getInstance().SaveBMPToFile(provincesBMP, (data->modPath + ("map/provinces.bmp")).c_str());
}
