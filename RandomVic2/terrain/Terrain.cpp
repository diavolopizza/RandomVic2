#include "Terrain.h"
#include "../utils/Random.h"
#include "../utils/BMPHandler.h"
#include "../FastNoise/FastNoise.h"
#include <windows.h> //for timeGetTime() mainly
//MACROS
#define LEFT(val) \
(val-3)
#define RIGHT(val) \
(val+3)
#define ABOVE(val, offset) \
(val+offset)
#define BELOW(val, offset) \
(val-offset)
#define ABOVE(val, offset) \
(val+offset)
#define BELOW(val, offset) \
(val-offset)

Terrain::Terrain(ranlux48* random, uint32_t generalBmpWidth, uint32_t generalBmpHeight)
{
	this->random = random;
	provinceMap.resize(boost::extents[256][256][256]);
	this->generalBmpHeight = generalBmpHeight;
	this->generalBmpWidth = generalBmpWidth;
}

Terrain::~Terrain()
{
}

void Terrain::determineStartingPixel(Bitmap* b, vector<uint32_t> &provincePixels, RGBTRIPLE &provinceColour) {
	uint32_t bmpWidth = b->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = b->bitmapinfoheader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	uint32_t startingPixel = (*random)() % bmpSize;//startingpixel is anywhere in the file
	while (!(startingPixel >= bmpWidth && startingPixel <= bmpSize - bmpWidth && b->getValueAt(startingPixel * 3) == provinceColour.rgbtBlue - 1))
	{
		startingPixel = (*random)() % bmpSize;//startingpixel is anywhere in the file
	}
	b->setTriple(provinceColour, startingPixel * 3);
	provincePixels.push_back(startingPixel * 3);
}

BYTE* Terrain::landProvinces(uint32_t numoflandprov, Bitmap * terrainBMP, Bitmap * provinceBMP)
{
	cout << "Landprovinces" << endl;
	uint32_t bmpWidth = terrainBMP->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = terrainBMP->bitmapinfoheader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	RGBTRIPLE rgbHigh{ 254, 254, 254 };
	RGBTRIPLE rgbLow{ 0, 0, 0 };
	BYTE* provinceBuffer = provinceBMP->Buffer;
	for (uint32_t i = 0; i < bmpSize * 3; i += 3) {
		if (terrainBMP->getValueAt(i / 3) == 254) {
			provinceBMP->setTriple(rgbHigh, i);
		}
		else {
			provinceBMP->setTriple(rgbLow, i);
		}
	}
	uint32_t provincesize = (int)((float)bmpSize * 0.2f / (float)numoflandprov);//better calculation?
	provinceCreation(provinceBMP, provincesize, numoflandprov, 0);
	fill(provinceBMP, 1, 0);
	assignRemainingPixels(provinceBMP, provinceBuffer, provinces, false);
	return provinceBuffer;
}

BYTE* Terrain::seaProvinces(uint32_t numOfSeaProv, uint32_t numoflandprov, Bitmap * terrainBMP, Bitmap * provinceBMP)
{
	uint32_t bmpWidth = terrainBMP->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = terrainBMP->bitmapinfoheader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	BYTE* provinceBuffer = provinceBMP->Buffer;
	cout << "Seaprovinces" << endl;
	uint32_t provincesize = bmpSize * 1 / numOfSeaProv;//better calculation?
	provinceCreation(provinceBMP, provincesize, numOfSeaProv, 254);
	fill(provinceBMP, 255, 254);
	assignRemainingPixels(provinceBMP, provinceBuffer, provinces, true);
	return provinceBuffer;
}
//creates the basic province with a random shape
void Terrain::provinceCreation(Bitmap * provinceBMP, uint32_t provinceSize, uint32_t numOfLandProv, uint32_t greyval)
{
	uint32_t bmpWidth = provinceBMP->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = provinceBMP->bitmapinfoheader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	uint32_t red = 0;
	uint32_t green = 0;
	RGBTRIPLE provinceColour;

	for (uint32_t i = 0; i < numOfLandProv; i++)
	{
		provinceColour.rgbtRed = 1 + red;
		provinceColour.rgbtGreen = 1 + green;
		provinceColour.rgbtBlue = greyval + 1;//land gets low blue value
		red++;
		if (red > 254)//if end of colourrange(255) is reached
		{
			green++; //increment second(g) value
			red = 0; //reset red
		}
		Prov* P = nullptr;
		if (provinceColour.rgbtBlue == 255)
		{
			P = new Prov(i, provinceColour, true, this->random); //create new landprovince
		}
		else {
			P = new Prov(i, provinceColour, false, this->random); //create new landprovince
		}
		determineStartingPixel(provinceBMP, P->pixels, provinceColour);
		for (uint32_t x = 0; x < provinceSize; x++)
		{
			uint32_t currentPixel = 0;
			while (currentPixel <= bmpWidth * 3 || currentPixel >= bmpSize * 3 - bmpWidth * 3)
			{
				currentPixel = P->pixels[(*random)() % P->pixels.size()];
			}
			if (provinceBMP->getValueAt(RIGHT(currentPixel)) == greyval)
			{
				provinceBMP->setTriple(provinceColour, RIGHT(currentPixel));
				P->pixels.push_back(RIGHT(currentPixel));
				x++;
			}
			if (provinceBMP->getValueAt(LEFT(currentPixel)) == greyval)
			{
				provinceBMP->setTriple(provinceColour, LEFT(currentPixel));
				P->pixels.push_back(LEFT(currentPixel));
				x++;
			}
			if (provinceBMP->getValueAt(ABOVE(currentPixel, bmpWidth * 3)) == greyval)
			{
				provinceBMP->setTriple(provinceColour, ABOVE(currentPixel, bmpWidth * 3));
				P->pixels.push_back(ABOVE(currentPixel, bmpWidth * 3));
				x++;
			}
			if (provinceBMP->getValueAt(BELOW(currentPixel, bmpWidth * 3)) == greyval)
			{
				provinceBMP->setTriple(provinceColour, BELOW(currentPixel, bmpWidth * 3));
				P->pixels.push_back(BELOW(currentPixel, bmpWidth * 3));
				x++;
			}
		}
		P->center = P->pixels[0];
		provinces.push_back(P);
	}
}
//fills unassigned pixels in iterations, so provinces grow
void Terrain::fill(Bitmap* provinceBMP, uint32_t greyVal, uint32_t fillVal)
{
	cout << "FILLING START" << endl;
	uint32_t bmpWidth = provinceBMP->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = provinceBMP->bitmapinfoheader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	uint32_t unassignedPixels = bmpSize;
	uint32_t previousUnassignedPixels = unassignedPixels + 1;
	while (unassignedPixels > 0 && unassignedPixels < previousUnassignedPixels)
	{
		if (unassignedPixels == 0)
			break;
		cout << unassignedPixels << endl;
		previousUnassignedPixels = unassignedPixels;
		unassignedPixels = 0;
		for (uint32_t unassignedPixel = 0; unassignedPixel < bmpSize * 3; unassignedPixel += 3)
		{
			if (provinceBMP->getValueAt(unassignedPixel) == fillVal)
			{
				unassignedPixels++;
				uint32_t direction = (*random)() % 4;
				switch (direction)
				{
				case 0: {
					if (unassignedPixel < bmpSize * 3 - 3 && provinceBMP->getValueAt(RIGHT(unassignedPixel)) != 0 && provinceBMP->getValueAt(RIGHT(unassignedPixel)) == greyVal)
					{
						provinceBMP->setTriple(unassignedPixel, RIGHT(unassignedPixel));
					}
					break;
				}
				case 1:
				{
					if (unassignedPixel > 3 && provinceBMP->getValueAt(LEFT(unassignedPixel)) != 0 && provinceBMP->getValueAt(LEFT(unassignedPixel)) == greyVal)
					{
						provinceBMP->setTriple(unassignedPixel, LEFT(unassignedPixel));
					}
					break;
				}
				case 2:
				{
					if (unassignedPixel < bmpSize * 3 - bmpWidth * 3 && provinceBMP->getValueAt(ABOVE(unassignedPixel, bmpWidth * 3)) != 0 && provinceBMP->getValueAt(ABOVE(unassignedPixel, bmpWidth * 3)) == greyVal)
					{
						provinceBMP->setTriple(unassignedPixel, ABOVE(unassignedPixel, bmpWidth * 3));
					}
					break;
				}
				case 3:
				{
					if (unassignedPixel > bmpWidth * 3 && provinceBMP->getValueAt(BELOW(unassignedPixel, bmpWidth * 3)) != 0 && provinceBMP->getValueAt(BELOW(unassignedPixel, bmpWidth * 3)) == greyVal)
					{
						provinceBMP->setTriple(unassignedPixel, BELOW(unassignedPixel, bmpWidth * 3));
					}
					break;
				}
				}
			}
		}
	}
}
//creates continents from the random landmasses and assigns
//provinces to those continents
void Terrain::evaluateContinents() {
	uint32_t continentID = 0;
	for (auto prov : provinces)
	{
		if (prov->continent == nullptr) {
			Continent *C = new Continent(to_string(continentID), continentID);
			continents.push_back(C);
			continentID++;
			prov->assignContinent(C);
		}
	}
	for (int i = 0; i < continents.size(); i++)
	{
		if (continents[i]->provinces.size() < 100) {
			for (auto province : continents[i]->provinces) {
				province->continent = nullptr;
			}
			continents.erase(continents.begin() + i);
			i--;
		}
	}
	for (auto prov : provinces)
	{
		if (prov->continent == nullptr && !prov->sea) {
			uint32_t distance = 10000;
			Continent* nextOwner = nullptr;
			for (Prov* P : provinces)
			{
				if (P->continent != nullptr) {
					uint32_t x1 = P->center % generalBmpWidth;
					uint32_t x2 = prov->center % generalBmpWidth;
					uint32_t y1 = P->center / generalBmpHeight;
					uint32_t y2 = prov->center / generalBmpHeight;
					if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
						distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
						nextOwner = P->continent;
					}
				}
			}
			prov->assignContinent(nextOwner);
		}
	}
}
//Finds neighbours of all provinces and assigns them
void Terrain::evaluateNeighbours(Bitmap * provinceBMP)
{
	uint32_t width = provinceBMP->bitmapinfoheader.biWidth;
	for (int i = 0; i < provinceBMP->bitmapinfoheader.biSizeImage - width * 3 - 9; i += 3)
	{
		provinceBMP->getTriple(i);
		Prov * left = provinceMap[provinceBMP->getTriple(i).rgbtRed][provinceBMP->getTriple(i).rgbtGreen][provinceBMP->getTriple(i).rgbtBlue];
		Prov * right = provinceMap[provinceBMP->getTriple(i + 3).rgbtRed][provinceBMP->getTriple(i + 3).rgbtGreen][provinceBMP->getTriple(i + 3).rgbtBlue];
		Prov* below = provinceMap[provinceBMP->getTriple(i + width * 3).rgbtRed][provinceBMP->getTriple(i + width * 3).rgbtGreen][provinceBMP->getTriple(i + width * 3).rgbtBlue];

		if (!(left == right)) {
			left->setneighbour2(right);
		}
		else if (!(left == below)) {
			left->setneighbour2(below);
		}
	}
}
//creates region of defined size on each continent and assigns
//provinces to those regions
void Terrain::evaluateRegions()
{
	uint32_t regionID = 0;
	for (auto prov : provinces)
	{
		if (prov->region == nullptr && !prov->sea) {
			Region *R = new Region(to_string(regionID), regionID);
			regions.push_back(R);
			regionID++;
			prov->assignRegion(R, true);
		}
	}
	for (int i = 0; i < regions.size(); i++)
	{
		if (regions[i]->provinces.size() < 3) {
			for (auto province : regions[i]->provinces) {
				province->region = nullptr;
			}
			regions.erase(regions.begin() + i);
			i--;
		}
	}
	for (auto prov : provinces)
	{
		if (prov->region == nullptr && !prov->sea) {
			uint32_t distance = 10000;
			Region* nextOwner = nullptr;
			for (Prov* P : provinces)
			{
				if (P->region != nullptr) {
					uint32_t x1 = P->center % generalBmpWidth;
					uint32_t x2 = prov->center % generalBmpWidth;
					uint32_t y1 = P->center / generalBmpHeight;
					uint32_t y2 = prov->center / generalBmpHeight;
					if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
						distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
						nextOwner = P->region;
					}
				}
			}
			prov->assignRegion(nextOwner, false);
		}
	}
}
void Terrain::evaluateCoasts(Bitmap * provinceBMP)
{
	for (auto prov : provinces) {
		if (!prov->sea) {
			for (auto pixel : prov->pixels) {
				if (provinceBMP->getValueAt(LEFT(pixel) == 254)) {
					prov->coastal = true;
				}
				else if (provinceBMP->getValueAt(RIGHT(pixel) == 254)) {
					prov->coastal = true;
				}
				else if (provinceBMP->getValueAt(BELOW(pixel, this->generalBmpWidth) == 254)) {
					prov->coastal = true;
				}
				else if (provinceBMP->getValueAt(ABOVE(pixel, this->generalBmpWidth) == 254)) {
					prov->coastal = true;
				}
			}
		}
	}
}
//creates the terrain, factoring in heightmap and some climate calculations
BYTE* Terrain::createTerrain(Bitmap * terrainBMP, BYTE* heightMapBuffer)
{
	cout << "Creating basic terrain from heightmap" << endl;
	BYTE* terrainBuffer = new BYTE[terrainBMP->bitmapinfoheader.biSizeImage];
	uint32_t width = terrainBMP->bitmapinfoheader.biWidth;
	uint32_t height = terrainBMP->bitmapinfoheader.biHeight;
	for (int x = 0; x < height; x++)
	{
		for (int y = 0; y < width; y++)
		{
			if (heightMapBuffer[(x * width + y) * 3] > 145) {
				terrainBuffer[(x * width + y)] = 13;
			}
			else {
				terrainBuffer[(x * width + y)] = 254;
			}
		}
	}
	return terrainBuffer;
}
//creates the heightmap with a given seed
BYTE* Terrain::heightMap(Bitmap * RGBBMP, uint32_t seed)
{
	cout << "Create Heightmap" << endl;
	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	//myNoise.SetSeed((*random)() % 1000000);
	myNoise.SetSeed(seed);
	myNoise.SetFrequency(0.0006);
	myNoise.SetFractalOctaves(11);
	myNoise.SetFractalGain(0.5);
	myNoise.SetFractalType(FastNoise::FBM);
	const int width = RGBBMP->bitmapinfoheader.biWidth;
	const int height = RGBBMP->bitmapinfoheader.biHeight;
	BYTE* heightmapBuffer = new BYTE[RGBBMP->bitmapinfoheader.biSizeImage];
	float delimiter = generalBmpWidth / 10;
	for (int x = 0; x < height; x++)
	{
		for (int y = 0; y < width; y++)
		{
			float factor = 1;
			if (y < delimiter) {
				factor = (float)y / (float)delimiter;
			}
			else if (y > width - delimiter)
			{
				factor = ((float)width - (float)y) / delimiter;
			}
			heightmapBuffer[(x * width + y) * 3] = (myNoise.GetNoise(x, y) + 1) * 128 * factor;
			heightmapBuffer[(x * width + y) * 3 + 1] = (myNoise.GetNoise(x, y) + 1) * 128 * factor;
			heightmapBuffer[(x * width + y) * 3 + 2] = (myNoise.GetNoise(x, y) + 1) * 128 * factor;
		}
	}
	return heightmapBuffer;
}
//assigns all unassigned pixels to the nearest province
void Terrain::assignRemainingPixels(Bitmap * provinceBMP, BYTE* provinceBuffer, vector<Prov*> provinces, bool sea) {
	//Assign remaining pixels
	uint32_t bmpWidth = provinceBMP->bitmapinfoheader.biWidth;
	uint32_t width = provinceBMP->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = provinceBMP->bitmapinfoheader.biHeight;
	uint32_t bmpSize = provinceBMP->bitmapinfoheader.biSizeImage;

	for (uint32_t unassignedPixel = 0; unassignedPixel < bmpSize; unassignedPixel += 3)
	{
		if (sea) {

			if (provinceBuffer[unassignedPixel] == 254 || provinceBuffer[unassignedPixel] == 0)
			{
				RGBTRIPLE lakeColour;
				bool unique = false;
				while (!unique) {
					lakeColour.rgbtBlue = 128;
					lakeColour.rgbtGreen = (*random)() % 256;
					lakeColour.rgbtRed = (*random)() % 256;
					if (provinceMap[lakeColour.rgbtRed][lakeColour.rgbtGreen][lakeColour.rgbtBlue] == nullptr)
					{
						unique = true;
					}
				}
				Prov * lake = new Prov(provinces.size(), lakeColour, true, this->random);
				provinceMap[lakeColour.rgbtRed][lakeColour.rgbtGreen][lakeColour.rgbtBlue] = lake;
				provinces.push_back(lake);
				lake->pixels.push_back(unassignedPixel);
				provinceBMP->setTriple(lakeColour, unassignedPixel);
				bool newFound = false;
				bool first = true;
				while (newFound || first) {
					newFound = false;
					first = false;
					for (auto i : lake->pixels) {
						if (i < bmpSize - width * 3 && provinceBuffer[ABOVE(i, width * 3)] == 0)
						{
							provinceBMP->setTriple(lakeColour, ABOVE(i, width * 3));
							//assignColour(lakeColour, provinceBuffer, ABOVE(i, width * 3), 1);
							lake->pixels.push_back(ABOVE(i, width * 3));
							newFound = true;
						}
						if (i > width * 3 && provinceBuffer[BELOW(i, width * 3)] == 0)
						{
							provinceBMP->setTriple(lakeColour, BELOW(i, width * 3));
							//assignColour(lakeColour, provinceBuffer, BELOW(i, width * 3), 1);
							lake->pixels.push_back(BELOW(i, width * 3));
							newFound = true;
						}
						if (i + 3 < bmpSize - 3 && provinceBuffer[i + 3] == 0)
						{
							provinceBMP->setTriple(lakeColour, i + 3);
							//assignColour(lakeColour, provinceBuffer, i + 3, 1);
							lake->pixels.push_back((i + 3));
							newFound = true;
						}
						if (i - 3 > 0 && provinceBuffer[i - 3] == 0)
						{
							provinceBMP->setTriple(lakeColour, i - 3);
							//assignColour(lakeColour, provinceBuffer, i - 3, 1);
							lake->pixels.push_back((i - 3));
							newFound = true;
						}
					}
				}
			}
		}
		else {
			if (provinceBuffer[unassignedPixel] == 0 || provinceBuffer[unassignedPixel] == 4)
			{
				uint32_t distance = 10000;
				Prov* nextOwner = nullptr;
				for (Prov* P : provinces)
				{
					if ((provinceBuffer[unassignedPixel] == 0 && P->colour.rgbtBlue == 1) || (provinceBuffer[unassignedPixel] == 254 && P->colour.rgbtBlue == 255)) {
						//length of vector between current pixel and province pixel
						/*uint32_t nearestPixelOfThatProvince = 0;
						uint32_t pixelDistance = INFINITY;
						for (int i = 0 ; i < (P->pixels.size()); i+=30)
						{
							if (i < P->pixels.size()) {
								uint32_t provincePixel = P->pixels[i];
								uint32_t x1 = provincePixel % bmpWidth;
								uint32_t x2 = unassignedPixel % bmpWidth;
								uint32_t y1 = provincePixel / bmpHeight;
								uint32_t y2 = unassignedPixel / bmpHeight;
								if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < pixelDistance) {
									pixelDistance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
									nearestPixelOfThatProvince = provincePixel;
								}
							}
						}*/
						uint32_t x1 = P->center % bmpWidth;
						//uint32_t x1 = nearestPixelOfThatProvince % bmpWidth;
						uint32_t x2 = unassignedPixel / 3 % bmpWidth;
						uint32_t y1 = P->center / bmpHeight;
						//uint32_t y1 = nearestPixelOfThatProvince / bmpHeight;
						uint32_t y2 = unassignedPixel / 3 / bmpHeight;
						if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
							distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
							nextOwner = P;
						}
					}
				}
				provinceBMP->setTriple(nextOwner->colour, unassignedPixel);
			}
		}
	}
}
//Reads Pixels from bitmap and assigns them to provinces
void Terrain::provPixels(Bitmap * provinceBMP)
{
	int r, g, b;
	for (int j = 0; j < provinceBMP->bitmapinfoheader.biSizeImage - 3; j += 3)
	{
		try {
			r = provinceBMP->Buffer[j + 2];
			g = provinceBMP->Buffer[j + 1];
			b = provinceBMP->Buffer[j];
			provinceMap[r][g][b]->pixels.push_back(j);
		}
		catch (runtime_error e)
		{
			cout << r << " " << g << " " << b << endl;
		}
	}
}
//writes the continents to a bitmap, non-unique colours
void Terrain::prettyContinents(Bitmap * continentBMP)
{
	delete continentBMP->Buffer;
	continentBMP->Buffer = new BYTE[continentBMP->bitmapinfoheader.biSizeImage];
	for (auto continent : continents) {
		cout << continent->provinces.size() << endl;
		RGBTRIPLE continentColour;
		continentColour.rgbtBlue = (*random)() % 256;
		continentColour.rgbtGreen = (*random)() % 256;
		continentColour.rgbtRed = (*random)() % 256;
		for (auto province : continent->provinces)
		{
			for (uint32_t pixel : province->pixels)
			{
				continentBMP->setTriple(continentColour, pixel);
			}
		}
	}
}
//writes the regions to a bitmap, non-unique colours
void Terrain::prettyRegions(Bitmap * regionBMP)
{
	//	delete regionBMP->Buffer;
	regionBMP->Buffer = new BYTE[regionBMP->bitmapinfoheader.biSizeImage];
	for (auto region : regions) {
		RGBTRIPLE regionColour;
		regionColour.rgbtBlue = (*random)() % 256;
		regionColour.rgbtGreen = (*random)() % 256;
		regionColour.rgbtRed = (*random)() % 256;
		for (auto province : region->provinces)
		{
			for (int pixel : province->pixels)
			{
				regionBMP->setTriple(regionColour, pixel);
			}
		}
	}
}
//evaluates province size to define wether it should be deleted
//in case it is too small
void Terrain::prettyProvinces(Bitmap * provinceBMP)
{
	for (auto province : provinces)
	{
		if (province->pixels.size() < 30 && !province->sea) {
			cout << "TINY PROVINCE" << endl;
			for (auto pixel : province->pixels) {
				RGBTRIPLE colour;
				colour.rgbtBlue = 0;
				colour.rgbtGreen = 0;
				colour.rgbtRed = 0;
				provinceBMP->setTriple(colour, pixel);
			}
			province->pixels.clear();
			provinceMap[province->colour.rgbtRed][province->colour.rgbtGreen][province->colour.rgbtBlue] = nullptr;
		}
	}
	for (int i = 0; i < provinces.size(); i++)
	{
		if (provinces[i]->pixels.size() == 0 && !provinces[i]->sea) {
			cout << "DELETE PROVINCE" << endl;
			provinces.erase(provinces.begin() + i);
			i--;
		}
	}
	fill(provinceBMP, 0, 0);
	assignRemainingPixels(provinceBMP, provinceBMP->Buffer, provinces, false);
	provPixels(provinceBMP);
}
void Terrain::prettyTerrain(Bitmap * terrainBMP, Bitmap * heightmap)
{
	uint32_t mountainStart = 185;
	uint32_t hillStart = 170;

	uint32_t arctic = 0;//0-3
	uint32_t plains = 5;
	uint32_t farmlands = 8;
	uint32_t forest = 12;
	uint32_t hills = 16;
	uint32_t woods = 20;
	uint32_t mountains = 24; //24-31
	uint32_t plains2 = 32;
	uint32_t steppe = 36;
	uint32_t jungle = 40;
	uint32_t marsh = 44;
	uint32_t desert = 48;
	uint32_t coastal_desert = 52;

	for (auto prov : provinces)
	{
		if (prov->sea)
			continue;
		vector<float> likelyHoods;
		float arcticLikelyhood = 0;
		float plainsLikelyhood = 0;
		float forestLikelyhood = 0;
		float farmlandsLikelyhood = 0;
		float steppeLikelyhood = 0;
		float jungleLikelyhood = 0;
		float marshLikelyhood = 0;
		float desertLikelyhood = 0;
		//determine north south temperature factor
		float equator = generalBmpHeight / 2;
		float pixX = prov->center / 3 % (generalBmpWidth);
		float pixY = (prov->center / 3 - pixX) / generalBmpWidth;
		float temperatureFactor = ((int)pixY % (int)equator) / equator;
		if (pixY >= equator) {
			temperatureFactor = 1 - temperatureFactor;
		}			if (temperatureFactor < 0.1 && !prov->sea)//mountains
		{
			arcticLikelyhood += 1;
		}
		if (temperatureFactor < 0.3 && temperatureFactor >= 0.1 && !prov->sea)
		{
			arcticLikelyhood += 0.3 - temperatureFactor;//0.2-0
			forestLikelyhood += 0.1 + temperatureFactor;//0.2-0.4
		}
		if (temperatureFactor < 0.6 && temperatureFactor >= 0.3 && !prov->sea)
		{
			farmlandsLikelyhood += temperatureFactor - 0.3;//0.0-0.3
			marshLikelyhood += 0.03;
			forestLikelyhood += (0.7 - temperatureFactor);//0.4-0.1
		}
		//if (temperatureFactor < 0.65 && temperatureFactor >= 0.5 && !prov->sea)
		//{
		//	farmlandsLikelyhood += 0.3;//0.3
		//	forestLikelyhood += 0.1;//0.1
		//}
		if (temperatureFactor < 0.75 && temperatureFactor >= 0.6 && !prov->sea)
		{
			farmlandsLikelyhood += 0.9 - temperatureFactor;//0.3-0.15
			steppeLikelyhood += temperatureFactor - 0.6;//0.0-0.15
			//desertLikelyhood += 0.15;
		}
		if (temperatureFactor < 0.85 && temperatureFactor >= 0.75 && !prov->sea)
		{
			steppeLikelyhood += temperatureFactor - 0.6;//0.15-0.25
			farmlandsLikelyhood += 0.1;
			//desertLikelyhood += 0.5;//
		}
		if (temperatureFactor >= 0.85 && !prov->sea)
		{
			steppeLikelyhood += (uint32_t)(1.0 - temperatureFactor);//0.25-0.1
			jungleLikelyhood += (temperatureFactor - 0.85) * 4;//0.0-0.7
			//farmlandsLikelyhood += 0.3;//0.0-0.3
		}
		//DISTANCE TO COAST
		uint32_t offset = 500;
		float distance = 0;
		uint32_t bitmapWidth = terrainBMP->bitmapinfoheader.biWidth;
		while (terrainBMP->getValueAt(prov->center) - offset == 254 && (prov->center - offset) % (bitmapWidth) > prov->center %bitmapWidth && offset > 0) {
			offset--;
			distance = offset;
			cout << "TRUE";
		}
		if (offset < 500)
			farmlandsLikelyhood += 10;


		//WESTWINDS



		//



		likelyHoods.push_back(arcticLikelyhood);
		likelyHoods.push_back(plainsLikelyhood);
		likelyHoods.push_back(forestLikelyhood);
		likelyHoods.push_back(farmlandsLikelyhood);
		likelyHoods.push_back(steppeLikelyhood);
		likelyHoods.push_back(jungleLikelyhood);
		likelyHoods.push_back(marshLikelyhood);
		likelyHoods.push_back(desertLikelyhood);
		float sumOfAllLikelyhoods = 0;
		uint32_t highestLikelyhoodIndex = 0;
		for (uint32_t i = 0; i < likelyHoods.size(); i++) {
			sumOfAllLikelyhoods += likelyHoods[i];
		}

		vector<bool> candidates;
		for (float likelyHood : likelyHoods) {
			candidates.push_back(false);
		}

		for (auto candidate : candidates) {
			candidate = false;
		}
		bool foundAtLeastOne = false;
		while (!foundAtLeastOne) {
			for (int i = 0; i < likelyHoods.size(); i++) {
				if ((*random)() % 100 < (likelyHoods[i] / sumOfAllLikelyhoods) * 100)
				{
					candidates[i] = true;
					foundAtLeastOne = true;
				}
			}
		}
		bool pick = false;
		uint32_t index = 0;
		while (!pick)
		{
			index = (*random)() % candidates.size();
			pick = candidates[index];
		}
		for (auto pixel : prov->pixels)
		{
			uint32_t altitude = heightmap->getValueAt(pixel);
			switch (index) {
			case 0:
			{
				terrainBMP->Buffer[pixel / 3] = arctic;
				prov->climate = "inhospitable_climate";
				break;
			}
			case 1:
			{
				terrainBMP->Buffer[pixel / 3] = plains;
				prov->climate = "temperate_climate";
				break;
			}
			case 2:
			{
				terrainBMP->Buffer[pixel / 3] = forest;
				prov->climate = "mild_climate";
				if (temperatureFactor < 0.3)
					prov->climate = "harsh_climate";
				break;
			}
			case 3:
			{
				terrainBMP->Buffer[pixel / 3] = farmlands;
				prov->climate = "mild_climate";
				break;
			}
			case 4:
			{
				terrainBMP->Buffer[pixel / 3] = steppe;
				prov->climate = "harsh_climate";
				break;
			}
			case 5:
			{
				terrainBMP->Buffer[pixel / 3] = jungle;
				prov->climate = "harsh_climate";
				break;
			}
			case 6:
			{
				terrainBMP->Buffer[pixel / 3] = marsh;
				prov->climate = "temperate_climate";
				break;
			}
			case 7:
			{
				terrainBMP->Buffer[pixel / 3] = desert;
				prov->climate = "inhospitable_climate";
				break;
			}
			}
			if (altitude > hillStart)//mountains
			{
				terrainBMP->Buffer[pixel / 3] = (float)(altitude - hillStart) / (float)(mountainStart - hillStart) *(float)4 + 16;
			}
			if (altitude > mountainStart)//mountains
			{
				terrainBMP->Buffer[pixel / 3] = (float)(altitude - mountainStart) / (float)(210 - mountainStart) *(float)7 + 24;
			}
		}
	}
}
void Terrain::prettyRivers(Bitmap * riverBMP, Bitmap * heightmap)
{
	River R;
	R.setSource((*random)() % riverBMP->bitmapinfoheader.biSizeImage / 3);

	//find destination
	//Find any Coastal province nearby


}
//creates the province map for fast access of provinces when only
//rgb values are available, removes need to search this province
boost::multi_array<Prov*, 3> Terrain::createProvinceMap()
{
	for (auto province : provinces) {
		provinceMap[province->colour.rgbtRed][province->colour.rgbtGreen][province->colour.rgbtBlue] = province;
	}
	return provinceMap;

	//for (unsigned int i = 0; i < provinces.size(); i++)
	//{
	//	getline(defi, temp1);
	//	pos = temp1.find_first_of(';', pos + 1);
	//	provnr = stoi(temp1.substr(0, temp1.find_first_of(';')));

	//	r = stoi(temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1)));//RED

	//	pos = temp1.find_first_of(';', pos + 1);
	//	g = stoi(temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1)));//GREEN

	//	pos = temp1.find_first_of(';', pos + 1);
	//	b = stoi(temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1)));//BLUE

	//	Prov*prov = new Prov(provnr, r, g, b);

	//	pos = temp1.find_first_of(';', pos + 1);
	//	string temp2 = temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1) - pos - 1);//land, sea or lake
	//	if (temp2 == "sea" || temp2 == "lake")
	//		prov->sea = true;
	//	else {
	//		prov->sea = false;
	//	}

	//	pos = temp1.find_first_of(';', pos + 1);
	//	if (temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1) - pos - 1) == "true")//coastal
	//		prov->coastal = true;
	//	else
	//		prov->coastal = false;



	//	pos = temp1.find_first_of(';', pos + 1);
	//	prov->terrain_type = temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1) - pos - 1); //terraintype

	//	pos = temp1.find_first_of(';', pos + 1);
	//	prov->continent = stoi(temp1.substr(pos + 1, temp1.find_first_of(';', pos + 1) - pos - 1));//continent

	//	pos = 0;
	//	orderedprov[prov->provnr] = prov;
	//	provs[prov->r][prov->g][prov->b] = prov;
	//}
}