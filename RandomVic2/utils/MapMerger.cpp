#include "MapMerger.h"



MapMerger::MapMerger()
{
}


MapMerger::~MapMerger()
{
}

Bitmap MapMerger::mergeHeightRiver(const Bitmap heightmapBMP, const Bitmap riverBMP)
{
	Bitmap returnedBMP(Data::getInstance().width, Data::getInstance().height, 24);
	for (uint32_t i = 0; i < Data::getInstance().width* Data::getInstance().height; i++)
	{
		if (riverBMP.getValueAtIndex(i) <= 10) {
			returnedBMP.setTripleAtIndex(riverBMP.getColourTableEntry(riverBMP.getValueAtIndex(i)), i);
		}
		else {
			RGBTRIPLE colour = heightmapBMP.getTripleAtIndex(i);
			if (colour.rgbtBlue <= Data::getInstance().seaLevel)
				returnedBMP.setTripleAtIndex({ 0,0,0 }, i);
			else {
				colour.rgbtRed = (colour.rgbtRed / 5) * 5;
				colour.rgbtGreen = (colour.rgbtGreen / 5) * 5;
				colour.rgbtBlue = (colour.rgbtBlue / 5) * 5;
				returnedBMP.setTripleAtIndex(colour, i);
			}
		}
	}
	return returnedBMP;
}

Bitmap MapMerger::mergeCountryRiverProvince(const Bitmap countryBMP, const Bitmap riverBMP, const Bitmap provinceBMP)
{
	Bitmap returnedBMP(Data::getInstance().width, Data::getInstance().height, 24);
	for (uint32_t i = 0; i < Data::getInstance().width* Data::getInstance().height; i++)
	{
		if (riverBMP.getValueAtIndex(i) <= 10) {
			returnedBMP.setTripleAtIndex(riverBMP.getColourTableEntry(riverBMP.getValueAtIndex(i)), i);
		}
		else {
			RGBTRIPLE colour = countryBMP.getTripleAtIndex(i);
			RGBTRIPLE colour2 = provinceBMP.getTripleAtIndex(i);
			colour.rgbtRed = BYTE(colour.rgbtRed *0.8 + colour2.rgbtRed * 0.2);
			colour.rgbtGreen = BYTE(colour.rgbtGreen *0.8 + colour2.rgbtGreen * 0.2);
			colour.rgbtBlue = BYTE(colour.rgbtBlue *0.8 + colour2.rgbtBlue * 0.2);
			returnedBMP.setTripleAtIndex(colour, i);
		}

	}
	return returnedBMP;
}