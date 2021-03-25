#include "Victoria2Parser.h"
#include "../utils/Parser.h"
#include "../utils/Data.h"
#include "../utils/BMPHandler.h"
#include "CountryCreation.h"
#include "Victoria2Parser.h"
#include "../entities/Prov.h"
#include "../terrain/Terrain.h"
#include "../terrain/Provinces.h"
#pragma once
class VictoriaModule
{
public:
	VictoriaModule(Data *data, Provinces *terraingenerator);
	~VictoriaModule();
	void dumpMapFiles(Data* data, Bitmap terrainbmp, Bitmap riverBMP, Bitmap continents, Bitmap regionBMP, Bitmap heightmapBMP, Bitmap provincesBMP);
};

