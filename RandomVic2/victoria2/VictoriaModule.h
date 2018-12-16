#include "Victoria2Parser.h"
#include "../utils/Parser.h"
#include "../utils/Data.h"
#include "../utils/BMPHandler.h"
#include "CountryCreation.h"
#include "Victoria2Parser.h"
#include "../entities/Prov.h"
#include "../terrain/Terrain.h"
#pragma once
class VictoriaModule
{
public:
	VictoriaModule(Data *data, Terrain *terraingenerator);
	~VictoriaModule();
	void dumpMapFiles(Data* data, Bitmap* terrainbmp, Bitmap* riverBMP, Bitmap* continents, Bitmap* regionBMP, Bitmap* heightmapBMP, Bitmap* provincesBMP);
};

