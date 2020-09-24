#pragma once

#include "Data.h"
#include "Bitmap.h"

class MapMerger
{
public:
	MapMerger();
	~MapMerger();


	Bitmap * mergeHeightRiver(const Bitmap* heightmapBMP, const Bitmap* riverBMP);
	Bitmap * mergeCountryRiverProvince(const Bitmap* countryBMP, const Bitmap* riverBMP, const Bitmap * provinceBMP);
};

