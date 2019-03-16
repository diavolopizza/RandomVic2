#pragma once

#include "Data.h"
#include "Bitmap.h"

class MapMerger
{
public:
	MapMerger();
	~MapMerger();


	Bitmap * mergeHeightRiver(Bitmap* heightmapBMP, Bitmap* riverBMP);
	Bitmap * mergeCountryRiverProvince(Bitmap* countryBMP, Bitmap* riverBMP, Bitmap * provinceBMP);
};

