#include "Victoria2Parser.h"
#include "../utils/Parser.h"
#include "../utils/Data.h"
#include "../utils/BMPHandler.h"
#include "VictoriaCountryCreation.h"
#include "Victoria2Parser.h"
#include "../entities/Province.h"
#include "../terrain/TerrainGenerator.h"
#include "../terrain/ProvinceGenerator.h"
#pragma once
class VictoriaModule
{
public:
	VictoriaModule(Data *data, ProvinceGenerator *terraingenerator);
	~VictoriaModule();
	void dumpMapFiles(Data* data, Bitmap terrainbmp, Bitmap riverBMP, Bitmap continents, Bitmap regionBMP, Bitmap heightmapBMP, Bitmap provincesBMP);
};

