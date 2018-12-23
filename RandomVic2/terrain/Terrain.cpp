#include "Terrain.h"
#include "../utils/BMPHandler.h"
#include "../FastNoise/FastNoise.h"
#include "boost/algorithm/clamp.hpp"
#include <windows.h>
#include <thread>


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

Terrain::Terrain(ranlux48* random)
{
	this->random = random;
	provinceMap.resize(boost::extents[256][256][256]);
}

Terrain::~Terrain()
{
}


uint32_t Terrain::GetMinDistanceToProvince(uint32_t position, uint32_t width, uint32_t height) {
	uint32_t distance = MAXUINT32;
	for (Prov* P : provinces)
	{
		uint32_t x1 = P->center / 3 % width;
		uint32_t x2 = position / 3 % width;
		uint32_t y1 = P->center / 3 / height;
		uint32_t y2 = position / 3 / height;
		if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
			distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
		}
	}
	return distance;
}
//for a given radius, get all pixels in the range around given position 
uint32_t checkRange(uint32_t radius, uint32_t heightPos, uint32_t widthPos, Bitmap * RGBBMP, uint32_t width, uint32_t height, uint32_t seaLevel) {
	//vector<uint32_t> inRangePixels;
	//inRangePixels.resize(20);
	//uint32_t distance = MAXUINT32;
	//uint32_t start = position - radius * width;
	//uint32_t end = position + radius * width;
	int index = 0;
	//if (end > width * height * 3)
	//	end = width * height * 3;
	//if (position - radius * width < 0)
	//	start = 0;
	uint32_t xPos = widthPos;
	uint32_t yPos = heightPos;
	uint32_t widthStart = boost::algorithm::clamp(xPos - radius, 0, width);
	uint32_t widthEnd = boost::algorithm::clamp(xPos + radius, 0, width);
	uint32_t heightStart = boost::algorithm::clamp(yPos - 10, 0, height);
	uint32_t heightEnd = boost::algorithm::clamp(yPos + 10, 0, height);

	for (uint32_t heightIndex = heightStart; heightIndex < heightEnd; heightIndex++)
	{
		for (uint32_t widthIndex = widthStart; widthIndex < widthEnd; widthIndex++)
		{
			if (RGBBMP->getValueAtPositions(heightIndex, widthIndex) < seaLevel) {
				//inRangePixels.push_back((heightIndex * widthIndex + widthIndex) * 3);
				index++;
			}
		}
	}

	//for (int i = start; i < end; i += 3)
	//{
	//	uint32_t x1 = i / 3 % width;
	//	uint32_t x2 = position / 3 % width;
	//	uint32_t y1 = i / 3 / height;
	//	uint32_t y2 = position / 3 / height;
	//	distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
	//	if (distance < radius && RGBBMP->getValueAt(i) < 90) {
	//		inRangePixels[index] = i;
	//		index++;
	//	}
	//}
	return index;
}
//Utility to find starting point of new province
void Terrain::determineStartingPixel(Bitmap* b, vector<uint32_t> &provincePixels, RGBTRIPLE &provinceColour, uint32_t provinceSize) {
	uint32_t bmpWidth = b->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = b->bitmapinfoheader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	uint32_t startingPixel = (*random)() % bmpSize;//startingpixel is anywhere in the file
	while (!(startingPixel >= bmpWidth * 3 && startingPixel <= bmpSize - bmpWidth * 3 && b->getValueAt(startingPixel * 3) == provinceColour.rgbtBlue - 1)
		&& !GetMinDistanceToProvince(startingPixel, bmpWidth, bmpHeight) < 3 * sqrt(provinceSize / (std::atan(1) * 4)))
	{
		startingPixel = (*random)() % bmpSize;//startingpixel is anywhere in the file
	}
	b->setTriple(provinceColour, startingPixel * 3);
	provincePixels.push_back(startingPixel * 3);
}
//generates all land provinces
BYTE* Terrain::landProvinces(uint32_t numoflandprov, Bitmap * terrainBMP, Bitmap * provinceBMP, uint32_t updateThreshold)
{
	cout << "Generating landprovinces" << endl;
	uint32_t bmpWidth = terrainBMP->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = terrainBMP->bitmapinfoheader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	RGBTRIPLE rgbHigh{ 254, 254, 254 };
	RGBTRIPLE rgbLow{ 0, 0, 0 };
	//initialize buffer
	BYTE* provinceBuffer = provinceBMP->getBuffer();
	for (uint32_t i = 0; i < bmpSize * 3; i += 3) {
		if (terrainBMP->getValueAt(i / 3) == 254) {
			//sea
			provinceBMP->setTriple(rgbHigh, i);
		}
		else {
			//land
			provinceBMP->setTriple(rgbLow, i);
		}
	}
	//assign province size
	uint32_t provincesize = (int)((float)bmpSize * 0.6f / (float)numoflandprov);//better calculation?
	//assign pixels to this new province
	provinceCreation(provinceBMP, provincesize, numoflandprov, 0, 0);
	//For multithreading: create vector of random values. Used for performance improvements, as ranlux48 is using locks, and new instances would remove determination.
	vector<uint32_t> randomValuesCached;
	for (uint32_t i = 0; i < provinceBMP->bitmapinfoheader.biSizeImage / 64; i++) {
		randomValuesCached.push_back((*random)() % 4);
	}
	uint32_t threadCount = 1;
	//decrement number of threads, until biSizeImage can be divided by threadCount without any rest
	while (provinceBMP->bitmapinfoheader.biSizeImage % threadCount != 0)
	{
		threadCount--;
	}
	const uint32_t numThreads = threadCount;
	std::vector<std::thread> threads;
	for (uint32_t i = 0; i < numThreads; ++i) {
		uint32_t from = i * (provinceBMP->bitmapinfoheader.biSizeImage / numThreads);
		uint32_t to = (i + 1) * (provinceBMP->bitmapinfoheader.biSizeImage / numThreads);
		threads.push_back(std::thread(&Terrain::fill, this, std::ref(provinceBMP), (uint32_t)1, (uint32_t)0, from, to, std::ref(randomValuesCached), updateThreshold));
	}
	//wait for threads to finish
	for (auto& t : threads) {
		t.join();
	}
	assignRemainingPixels(provinceBMP, provinceBuffer, false);
	return provinceBuffer;
}
//generates all land provinces
BYTE* Terrain::seaProvinces(uint32_t numOfSeaProv, uint32_t numoflandprov, Bitmap * terrainBMP, Bitmap * provinceBMP, uint32_t updateThreshold)
{
	uint32_t bmpWidth = terrainBMP->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = terrainBMP->bitmapinfoheader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	BYTE* provinceBuffer = provinceBMP->getBuffer();
	cout << "Generating seaprovinces" << endl;
	uint32_t provincesize = bmpSize / numOfSeaProv;//better calculation?
	provinceCreation(provinceBMP, provincesize, numOfSeaProv, numoflandprov, 254);
	//multithreading
	uint32_t threadCount = 1;
	vector<uint32_t> randomValuesCached;
	for (uint32_t i = 0; i < provinceBMP->bitmapinfoheader.biSizeImage / 64; i++) {
		randomValuesCached.push_back((*random)() % 4);
	}
	while (provinceBMP->bitmapinfoheader.biSizeImage % threadCount != 0)
	{
		threadCount--;
	}
	const uint32_t numThreads = threadCount;
	std::vector<std::thread> threads;
	for (uint32_t i = 0; i < numThreads; ++i) {
		uint32_t from = i * (provinceBMP->bitmapinfoheader.biSizeImage / numThreads);
		uint32_t to = (i + 1) * (provinceBMP->bitmapinfoheader.biSizeImage / numThreads);
		threads.push_back(std::thread(&Terrain::fill, this, std::ref(provinceBMP), (uint32_t)255, (uint32_t)254, from, to, std::ref(randomValuesCached), updateThreshold));
	}
	//wait for threads to finish
	for (auto& t : threads) {
		t.join();
	}
	//fill(provinceBMP, 255, 254, 0);
	assignRemainingPixels(provinceBMP, provinceBuffer, true);
	return provinceBuffer;
}
//creates the basic province with a random shape
void Terrain::provinceCreation(Bitmap * provinceBMP, uint32_t provinceSize, uint32_t numOfLandProv, uint32_t offset, uint32_t greyval)
{
	uint32_t bmpWidth = provinceBMP->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = provinceBMP->bitmapinfoheader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	uint32_t red = 0;
	uint32_t green = 0;
	RGBTRIPLE provinceColour;

	for (uint32_t i = offset + 1; i < numOfLandProv + offset + 1; i++)
	{
		provinceColour.rgbtRed = 1 + red;
		provinceColour.rgbtGreen = 1 + green;
		provinceColour.rgbtBlue = greyval + 1;//land gets low blue value
		red++;
		if (red > 254)//if end of colourrange(255) is reached
		{
			green++; //increment second(g) value
			red = 1; //reset red
		}
		Prov* P = new Prov(i, provinceColour, provinceColour.rgbtBlue == 255, this->random); //create new landprovince

		determineStartingPixel(provinceBMP, P->pixels, provinceColour, provinceSize);
		for (uint32_t x = 0; x < provinceSize - 1; x++)
		{
			uint32_t currentPixel = 0;
			while (currentPixel <= bmpWidth * 3 || currentPixel >= bmpSize * 3 - bmpWidth * 3)
			{
				currentPixel = P->pixels[(*random)() % P->pixels.size()];
			}
			if (provinceBMP->getValueAt(RIGHT(currentPixel)) == greyval)
			{
				if (((currentPixel / 3) + 1) % (bmpWidth / 2) != 0) {
					provinceBMP->setTriple(provinceColour, RIGHT(currentPixel));
					P->pixels.push_back(RIGHT(currentPixel));
					x++;
				}
			}
			if (provinceBMP->getValueAt(LEFT(currentPixel)) == greyval)
			{
				if (((currentPixel / 3)) % (bmpWidth / 2) != 0) {
					provinceBMP->setTriple(provinceColour, LEFT(currentPixel));
					P->pixels.push_back(LEFT(currentPixel));
					x++;
				}
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
		if (P->pixels.size() < 1)
			cout << "´Zero size province" << endl;
		provinces.push_back(P);
	}
}
//fills unassigned pixels in iterations, so provinces grow
void Terrain::fill(Bitmap* provinceBMP, uint32_t greyVal, uint32_t fillVal, uint32_t from, uint32_t to, vector<uint32_t> &randomValuesCached, uint32_t updateThreshold)
{
	Visualizer::initializeWindow();
	cout << "Starting filling of unassigned pixels from pixel " << from << " to pixel " << to << endl;
	uint32_t bmpWidth = provinceBMP->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = provinceBMP->bitmapinfoheader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	uint32_t unassignedPixels = bmpSize;
	uint32_t previousUnassignedPixels = unassignedPixels + 1;
	uint32_t randomValueIndex = 0;
	uint32_t imageDrawn = 0;
	while (unassignedPixels > 0 && unassignedPixels < previousUnassignedPixels)
	{
		if (unassignedPixels == 0)
			break;
		cout << "Pixels still unassigned: " << unassignedPixels << endl;
		previousUnassignedPixels = unassignedPixels;
		unassignedPixels = 0;
		for (uint32_t unassignedPixel = from; unassignedPixel < to; unassignedPixel += 3)
		{
			if (provinceBMP->getValueAt(unassignedPixel) == fillVal)
			{
				unassignedPixels++;
				uint32_t direction = randomValuesCached[randomValueIndex++];
				if (randomValueIndex >= randomValuesCached.size())
					randomValueIndex = 0;
				switch (direction)
				{
					//Constraints when checking neighbours:
						//not going out of buffer bounds
						//only assign if neighbouring pixel is assigned and of same type
						//not crossing the wrapping line in east/west direction
				case 0: {
					if (unassignedPixel < bmpSize * 3 - 3 && provinceBMP->getValueAt(RIGHT(unassignedPixel)) != 0 && provinceBMP->getValueAt(RIGHT(unassignedPixel)) == greyVal)
					{
						if (((unassignedPixel / 3) + 1) % (bmpWidth) != 0)
							provinceBMP->setTriple(unassignedPixel, RIGHT(unassignedPixel));
					}
					break;
				}
				case 1:
				{
					if (unassignedPixel > 3 && provinceBMP->getValueAt(LEFT(unassignedPixel)) != 0 && provinceBMP->getValueAt(LEFT(unassignedPixel)) == greyVal)
					{
						if ((unassignedPixel / 3) % (bmpWidth) != 0)
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
		if (unassignedPixels < imageDrawn - updateThreshold * 50) {
			Visualizer::displayImage(provinceBMP);
			imageDrawn = unassignedPixels;
		}
	}
	cvDestroyAllWindows();
}
//creates continents from the random landmasses and assigns
//provinces to those continents
void Terrain::evaluateContinents(uint32_t minProvPerContinent, uint32_t width, uint32_t height) {
	uint32_t continentID = 0;

	//for (auto region : regions)
	//{
	//	if (region->continent == nullptr)
	//	{
	//		Continent *C = new Continent(to_string(continentID), continentID);
	//		continents.push_back(C);
	//		continentID++;
	//		region->assignContinent(C, 0, minRegionPerContinent);
	//	}
	//}

	for (auto prov : provinces)
	{
		if (prov->continent == nullptr && !prov->sea) {
			Continent *C = new Continent(to_string(continentID), continentID);
			continents.push_back(C);
			continentID++;
			prov->assignContinent(C);
		}
	}
	for (uint32_t i = 0; i < continents.size(); i++)
	{
		if (continents[i]->provinces.size() < minProvPerContinent) {
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
			uint32_t distance = MAXUINT32;
			Continent* nextOwner = nullptr;
			for (Prov* P : provinces)
			{
				if (P->continent != nullptr) {
					uint32_t x1 = P->center / 3 % width;
					uint32_t x2 = prov->center / 3 % width;
					uint32_t y1 = P->center / 3 / height;
					uint32_t y2 = prov->center / 3 / height;
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
	for (uint32_t i = 0; i < provinceBMP->bitmapinfoheader.biSizeImage - width * 3 - 9; i += 3)
	{
		provinceBMP->getTriple(i);
		Prov * left = provinceMap[provinceBMP->getTriple(i).rgbtRed][provinceBMP->getTriple(i).rgbtGreen][provinceBMP->getTriple(i).rgbtBlue];
		Prov * right = provinceMap[provinceBMP->getTriple(i + 3).rgbtRed][provinceBMP->getTriple(i + 3).rgbtGreen][provinceBMP->getTriple(i + 3).rgbtBlue];
		Prov* below = provinceMap[provinceBMP->getTriple(i + width * 3).rgbtRed][provinceBMP->getTriple(i + width * 3).rgbtGreen][provinceBMP->getTriple(i + width * 3).rgbtBlue];

		if (!(left == right)) {
			left->setNeighbour(right);
		}
		else if (!(left == below)) {
			left->setNeighbour(below);
		}
	}
}
//creates region of defined size on each continent and assigns
//provinces to those regions
void Terrain::evaluateRegions(uint32_t minProvPerRegion, uint32_t width, uint32_t height)
{
	uint32_t regionID = 0;
	for (auto prov : provinces)
	{
		if (prov->region == nullptr && !prov->sea) {
			Region *R = new Region(to_string(regionID), regionID);
			regions.push_back(R);
			regionID++;
			prov->assignRegion(R, true, minProvPerRegion);
		}
	}
	for (uint32_t i = 0; i < regions.size(); i++)
	{
		if (regions[i]->provinces.size() < minProvPerRegion) {
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
			uint32_t distance = MAXUINT32;
			Region* nextOwner = nullptr;
			for (Prov* P : provinces)
			{
				if (P->region != nullptr) {
					uint32_t x1 = P->center / 3 % width;
					uint32_t x2 = prov->center / 3 % width;
					uint32_t y1 = P->center / 3 / height;
					uint32_t y2 = prov->center / 3 / height;
					if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
						distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
						nextOwner = P->region;
					}
				}
			}
			if (nextOwner != nullptr)
				prov->assignRegion(nextOwner, false, minProvPerRegion);
		}
	}
	for (auto region : regions)
	{
		for (auto prov : region->provinces)
		{
			for (auto prov2 : prov->neighbourProvinces) {
				if (!prov2->sea && prov2->region != region)
				{
					region->setNeighbour(prov2->region);
				}
			}
		}
	}

}
//evaluate if province is coastal
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
				else if (provinceBMP->getValueAt(BELOW(pixel, provinceBMP->bitmapinfoheader.biWidth) == 254)) {
					prov->coastal = true;
				}
				else if (provinceBMP->getValueAt(ABOVE(pixel, provinceBMP->bitmapinfoheader.biWidth) == 254)) {
					prov->coastal = true;
				}
			}
		}
	}
}
//creates the terrain, factoring in heightmap and some climate calculations
BYTE* Terrain::createTerrain(Bitmap * terrainBMP, BYTE* heightMapBuffer, uint32_t &seaLevel, double landPercentage)
{
	double tempLandPercentage = 0;
	uint32_t landPixels = 0;
	BYTE* terrainBuffer = terrainBMP->getBuffer();// new BYTE[terrainBMP->bitmapinfoheader.biSizeImage];
	while (tempLandPercentage < landPercentage) {
		seaLevel--;
		cout << "Creating basic terrain from heightmap" << endl;
		uint32_t width = terrainBMP->bitmapinfoheader.biWidth;
		uint32_t height = terrainBMP->bitmapinfoheader.biHeight;
		for (uint32_t x = 0; x < height; x++)
		{
			for (uint32_t y = 0; y < width; y++)
			{
				if (heightMapBuffer[(x * width + y) * 3] > seaLevel) {
					terrainBuffer[(x * width + y)] = 13;
					landPixels++;
				}
				else {
					terrainBuffer[(x * width + y)] = 254;
				}
			}
		}
		tempLandPercentage = (double)landPixels / (double)(terrainBMP->bitmapinfoheader.biSizeImage);
		cout << "Landpercentage: " << tempLandPercentage << endl;
		landPixels = 0;

	}
	return terrainBuffer;
}
//creates the heightmap with a given seed
BYTE* Terrain::heightMap(Bitmap * RGBBMP, uint32_t seed, float frequency, uint32_t fractalOctaves, float fractalGain, uint32_t divideThreshold, uint32_t seaLevel, bool complexHeight, uint32_t updateThreshold)
{
	Visualizer::initializeWindow();
	char redo;
	/*do {*/
	cout << "Creating Heightmap" << endl;
	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetNoiseType(FastNoise::NoiseType::SimplexFractal); // Set the desired noise type

	if (seed)
		myNoise.SetSeed(seed);
	else
		myNoise.SetSeed(time(NULL));
	myNoise.SetFrequency(frequency);
	myNoise.SetFractalOctaves(fractalOctaves);
	myNoise.SetFractalGain(fractalGain);
	myNoise.SetFractalType(FastNoise::FBM);

	FastNoise myNoise2; // Create a FastNoise object
	myNoise2.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	myNoise2.SetSeed(3);
	myNoise2.SetFrequency(0.0036f);
	myNoise2.SetFractalOctaves(fractalOctaves);
	myNoise2.SetFractalGain(fractalGain);
	myNoise2.SetFractalType(FastNoise::FBM);

	const uint32_t width = RGBBMP->bitmapinfoheader.biWidth;
	const uint32_t height = RGBBMP->bitmapinfoheader.biHeight;
	uint32_t delimiter = width / divideThreshold;
	for (uint32_t x = 0; x < height; x++)
	{
		for (uint32_t y = 0; y < width; y++)
		{
			float factor = 1;
			if (y < delimiter) {
				factor = (float)y / (float)delimiter;
			}
			else if (y > width - delimiter)
			{
				factor = ((float)width - (float)y) / delimiter;
			}
			FN_DECIMAL noiseLevel = (myNoise.GetNoise(x, y) + 1) * 128 * factor;
			FN_DECIMAL completeNoise;
			if (noiseLevel >= seaLevel && complexHeight) {
				FN_DECIMAL noiseLevel2 = (myNoise2.GetNoise(x, y) + 1) * 128 * factor;
				completeNoise = (noiseLevel + (noiseLevel2 * 0.5)) *0.67;
				if (completeNoise < seaLevel + 1)
					completeNoise = seaLevel + 1;
			}
			else {
				completeNoise = noiseLevel;
			}

			RGBBMP->setSingle((x * width + y) * 3, completeNoise);
			RGBBMP->setSingle((x * width + y) * 3 + 1, completeNoise);
			RGBBMP->setSingle((x * width + y) * 3 + 2, completeNoise);
		}
		if (x % updateThreshold == 0)
			Visualizer::displayImage(RGBBMP);
	}
	/*'n' >> redo;*/
//} /*while (redo == 'y');*/
	cvDestroyAllWindows();
	/*uint32_t maxHeight = 256;
	double maxFactor = pow(maxHeight - seaLevel, 2);
	for (int i = 0; i < RGBBMP->bitmapinfoheader.biSizeImage; i++)
	{
		uint32_t positionHeight = RGBBMP->getValueAt(i);
		if (positionHeight > seaLevel)
		{
			if (seaLevel + (positionHeight *(pow(positionHeight - seaLevel, 2) / maxFactor) > maxHeight))
				RGBBMP->setSingle(i, maxHeight);

			RGBBMP->setSingle(i, seaLevel + (positionHeight *(pow(positionHeight - seaLevel, 2) / maxFactor)));
		}
	}*/
	return RGBBMP->getBuffer();
}

double calcMountainShadowAridity(Bitmap * heightmapBMP, uint32_t heightPos, uint32_t widthPos, int currentDirection, uint32_t seaLevel, double windIntensity) {
	uint32_t width = heightmapBMP->bitmapinfoheader.biWidth;
	//in regions with low windintensity, this effect has a lower range
	uint32_t maxEffectDistance = ((double)width / 100.0) * windIntensity;
	uint32_t mountainPixelsInRange = 0;
	for (int i = 0; i < maxEffectDistance; i++)
	{
		if (heightmapBMP->getValueAt((heightPos * width + widthPos + (currentDirection * i)) * 3) > seaLevel * 1.4)
		{
			mountainPixelsInRange++;
		}
	}
	return windIntensity * ((double)mountainPixelsInRange / (double)maxEffectDistance);
}

double calcCoastalHumidity(Bitmap * heightmapBMP, uint32_t heightPos, uint32_t widthPos, int currentDirection, uint32_t seaLevel, double windIntensity, uint32_t width, uint32_t height) {
	//the more continental, the less of an influence the distance to a coast has
	uint32_t continentality = 0;
	//East/west directions are more important that north/south, as important winds travel east/west more often
	uint32_t maxEffectDistance = width / 200;
	//in the opposite direction of the major global winds, the distance to look at is much larger
	uint32_t windFactor = 20 * windIntensity;
	uint32_t coastDistance = maxEffectDistance;
	uint32_t windDistance = maxEffectDistance * windFactor;

	//the direction opposite to the winds, e.g. west in case of west winds(winds coming from the west)
	for (int i = 0; i < maxEffectDistance * windFactor; i++)
	{
		int value = heightmapBMP->getValueAtPositions(heightPos, widthPos + (i * currentDirection));
		if (value != -1 && value < seaLevel)
		{
			if (i < windDistance)
				windDistance = i;
		}

	}
	for (int x = 0; x < maxEffectDistance * windFactor; x++) {
		int value = heightmapBMP->getValueAtPositions(heightPos, widthPos + ((windDistance + x) * currentDirection));
		if (value != -1 && value > seaLevel) {
			continentality++;
		}
	}
	//the direction opposite to the winds, e.g. west in case of west winds(winds coming from the west)
	for (int i = 0; i < maxEffectDistance; i++)
	{
		int value = heightmapBMP->getValueAtPositions(heightPos, widthPos + (i * currentDirection));
		if (value != -1 && value < seaLevel)
		{
			if (i < coastDistance)
				coastDistance = i;
		}

	}
	//the direction the winds are going(e.g. east in case of west winds)
	for (int i = 0; i < maxEffectDistance; i++)
	{
		int value = heightmapBMP->getValueAtPositions(heightPos, widthPos + (i * currentDirection * -1));
		if (value != -1 && value < seaLevel)
		{
			if (i < coastDistance)
				coastDistance = i;
		}
	}
	//north
	for (int i = 0; i < maxEffectDistance; i++)
	{
		int value = heightmapBMP->getValueAtPositions(heightPos + i, widthPos);
		if (value != -1 && value < seaLevel)
		{
			if (i < coastDistance)
				coastDistance = i;
		}
	}
	//south
	for (int i = 0; i < maxEffectDistance; i++)
	{
		int value = heightmapBMP->getValueAtPositions(heightPos - i, widthPos);
		if (value != -1 && value < seaLevel)
		{
			if (i < coastDistance)
				coastDistance = i;
		}
	}
	//the more is returned, the more humid the pixel
	//find the smallest factor:
		//higher means less humid
	double continentalityFactor = 1 * (((double)continentality / ((double)maxEffectDistance* (double)windFactor)));
	//higher means less humid
	double windDistanceFactor = (((double)windDistance / ((double)maxEffectDistance * (double)windFactor)));// *windIntensity;
	//higher means less humid
	double coastDistanceFactor = (double)coastDistance / (double)maxEffectDistance;
	if (windDistanceFactor + continentalityFactor > coastDistanceFactor)
		return 1 - coastDistanceFactor;
	else return 1 - (windDistanceFactor + continentalityFactor);

}

BYTE * Terrain::humidityMap(Bitmap * heightmapBMP, Bitmap* humidityBMP, uint32_t seaLevel, uint32_t updateThreshold)
{
	Visualizer::initializeWindow();
	cout << "Creating humidity map" << endl;
	double polarEasterlies = 0.3;
	double westerlies = 0.7;
	double tradeWinds = 1;
	const uint32_t width = heightmapBMP->bitmapinfoheader.biWidth;
	const uint32_t height = heightmapBMP->bitmapinfoheader.biHeight;

	for (uint32_t x = 0; x < height; x++)
	{
		int windDirection = 1;
		//the height of the image scaled between 0 and 2
		double heightf = (double)x / ((double)height / 2);

		double windIntensity = 0;
		//if the heightf is ...
		if (heightf == boost::algorithm::clamp(heightf, polarEasterlies, westerlies) || heightf == boost::algorithm::clamp(heightf, (double)(2 - westerlies), (double)(2 - polarEasterlies)))
		{   // ... in westerlies range
			windDirection = -1;
			if (heightf == boost::algorithm::clamp(heightf, polarEasterlies, westerlies))
				windIntensity = 0.4 + (0.6 * (1 - ((heightf - polarEasterlies) / (westerlies - polarEasterlies))));
			else if (heightf == boost::algorithm::clamp(heightf, (double)(2 - westerlies), (double)(2 - polarEasterlies))) {
				double a = (heightf - (1.99 - westerlies));
				double b = (2 - polarEasterlies) - (1.99 - westerlies);
				windIntensity = 0.4 + (0.6 * ((a / b)));
			}
		}
		else if (heightf == boost::algorithm::clamp(heightf, westerlies, tradeWinds) || heightf == boost::algorithm::clamp(heightf, (double)(2 - tradeWinds), (double)(2 - westerlies)))
		{   //... in tradewinds range
			windDirection = 1;
			if (heightf == boost::algorithm::clamp(heightf, westerlies, tradeWinds))
				windIntensity = 0.4 + (0.6 * (((heightf - westerlies) / (tradeWinds - westerlies))));
			else if (heightf == boost::algorithm::clamp(heightf, (double)(2 - tradeWinds), (double)(2 - westerlies))) {
				double a = (heightf - (1.99 - tradeWinds));
				double b = (2 - westerlies) - (1.99 - tradeWinds);
				windIntensity = 0.4 + (0.6 * (1 - (a / b)));
			}
		}
		else if (heightf == boost::algorithm::clamp(heightf, 0, polarEasterlies) || heightf == boost::algorithm::clamp(heightf, (double)(2 - polarEasterlies), (double)(2)))
		{   // ... in polar easterlies range
			windDirection = 1;
			if (heightf == boost::algorithm::clamp(heightf, 0, polarEasterlies))
				windIntensity = 0.4 + (0.6 * (((heightf) / (polarEasterlies))));
			else if (heightf == boost::algorithm::clamp(heightf, (double)(2 - polarEasterlies), (double)(2))) {
				double a = (heightf - (1.99 - polarEasterlies));
				double b = (2) - (1.99 - polarEasterlies);
				windIntensity = 0.4 + (0.6 * (1 - (a / b)));
			}
		}
		for (uint32_t y = 0; y < width; y++)
		{
			if (heightmapBMP->getValueAt((x * width + y) * 3) > seaLevel)
			{
				//the higher, the drier
				double mountainShadowAridity = calcMountainShadowAridity(heightmapBMP, x, y, windDirection, seaLevel, windIntensity);
				//the higher, the damper
				double coastalHumidity = calcCoastalHumidity(heightmapBMP, x, y, windDirection, seaLevel, windIntensity, width, height);
				coastalHumidity = boost::algorithm::clamp(coastalHumidity, 0, 1);
				//the higher, the drier
				double heatAridity = 0;

				if (heightf < 1)
				{
					double dryArea = westerlies + ((tradeWinds - westerlies) / 4);
					heatAridity = 0.5 - abs(heightf - dryArea);
					heatAridity *= 2;
					heatAridity = boost::algorithm::clamp(heatAridity, 0, 1);
					if (abs(heightf - tradeWinds) < 0.3)
					{
						heatAridity -= 0.3 - abs(heightf - tradeWinds);
					}
				}
				else {
					double dryArea = 2 - (westerlies + ((tradeWinds - westerlies) / 4));
					heatAridity = 0.5 - abs(heightf - dryArea);
					heatAridity *= 2;
					heatAridity = boost::algorithm::clamp(heatAridity, 0, 1);
					if (abs(heightf - tradeWinds) < 0.3)
					{
						heatAridity -= 0.3 - abs(heightf - tradeWinds);
					}
				}

				double totalAridity = 0;
				//incorporate temperature
				totalAridity += heatAridity;
				//incorporate the fact that precipitation falls on mountains and is not carried inwards
				totalAridity += mountainShadowAridity / 2;
				totalAridity = totalAridity * 0.2 + 0.8 * (totalAridity * (1 - coastalHumidity));
				//cannot get drier than super dry or less dry than humid
				totalAridity = boost::algorithm::clamp(totalAridity, 0, 1);

				{
					RGBTRIPLE colour = { 255.0 * (1.0 - totalAridity),totalAridity * 255,totalAridity * 255 };
					humidityBMP->setTriple(colour, (x * width + y) * 3);
				}
			}
		}
		if (x % updateThreshold == 0)
			Visualizer::displayImage(humidityBMP);
	}
	Bitmap tempBMP(width, height, 24, nullptr);
	for (int i = 0; i < width*height * 3; i++)
	{
		tempBMP.getBuffer()[i] = humidityBMP->getBuffer()[i];
	}
	uint32_t smoothDistance = 2;
	for (int i = width * 3 * smoothDistance + smoothDistance; i < humidityBMP->bitmapinfoheader.biSizeImage - width * 3 * smoothDistance - smoothDistance; i += 3)
	{
		RGBTRIPLE north = humidityBMP->getTriple(i - width * 3 * smoothDistance);
		RGBTRIPLE northwest = humidityBMP->getTriple(i - width * 3 * smoothDistance - smoothDistance * 3);
		RGBTRIPLE northeast = humidityBMP->getTriple(i - width * 3 * smoothDistance + smoothDistance * 3);
		RGBTRIPLE south = humidityBMP->getTriple(i + width * 3 * smoothDistance);
		RGBTRIPLE southwest = humidityBMP->getTriple(i + width * 3 * smoothDistance - smoothDistance * 3);
		RGBTRIPLE southeast = humidityBMP->getTriple(i + width * 3 * smoothDistance + smoothDistance * 3);
		RGBTRIPLE west = humidityBMP->getTriple(i + smoothDistance * 3);
		RGBTRIPLE east = humidityBMP->getTriple(i - smoothDistance * 3);
		RGBTRIPLE colour;
		colour.rgbtBlue = (double)((int)north.rgbtBlue + (int)northwest.rgbtBlue + (int)northeast.rgbtBlue + (int)southwest.rgbtBlue + (int)southeast.rgbtBlue + (int)south.rgbtBlue + (int)west.rgbtBlue + (int)east.rgbtBlue) / 8;
		colour.rgbtGreen = (double)((int)north.rgbtGreen + (int)northwest.rgbtGreen + (int)northeast.rgbtGreen + (int)southwest.rgbtGreen + (int)southeast.rgbtGreen + (int)south.rgbtGreen + (int)west.rgbtGreen + (int)east.rgbtGreen) / 8;
		colour.rgbtRed = (double)((int)north.rgbtRed + (int)northwest.rgbtRed + (int)northeast.rgbtRed + (int)southwest.rgbtRed + (int)southeast.rgbtRed + (int)south.rgbtRed + (int)west.rgbtRed + (int)east.rgbtRed) / 8;

		humidityBMP->setTriple(colour, i);
	}
	return humidityBMP->getBuffer();
}
//assigns all unassigned pixels to the nearest province
void Terrain::assignRemainingPixels(Bitmap * provinceBMP, BYTE* provinceBuffer, bool sea) {
	cout << "Assigning remaining pixels provinces" << endl;
	//Assign remaining pixels
	uint32_t bmpWidth = provinceBMP->bitmapinfoheader.biWidth;
	uint32_t width = provinceBMP->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = provinceBMP->bitmapinfoheader.biHeight;
	uint32_t bmpSize = provinceBMP->bitmapinfoheader.biSizeImage;

	for (uint32_t unassignedPixel = 0; unassignedPixel < bmpSize; unassignedPixel += 3)
	{
		if (sea) {
			if (provinceBuffer[unassignedPixel] == 254)
			{
				//find unique colour
				RGBTRIPLE lakeColour;
				bool unique = false;
				while (!unique) {
					lakeColour.rgbtBlue = 204;
					lakeColour.rgbtGreen = 1 + (*random)() % 255;
					lakeColour.rgbtRed = 1 + (*random)() % 255;
					if (provinceMap[lakeColour.rgbtRed][lakeColour.rgbtGreen][lakeColour.rgbtBlue] == nullptr)
					{
						unique = true;
					}
				}
				Prov * lake = new Prov(provinces.size() + 1, lakeColour, true, this->random);
				provinceMap[lakeColour.rgbtRed][lakeColour.rgbtGreen][lakeColour.rgbtBlue] = lake;
				provinces.push_back(lake);
				lake->pixels.push_back(unassignedPixel);
				lake->center = unassignedPixel;
				provinceBMP->setTriple(lakeColour, unassignedPixel);
				bool newFound = false;
				bool first = true;
				while (newFound || first) {
					newFound = false;
					first = false;
					for (uint32_t index = 0; index < lake->pixels.size(); index++) {
						uint32_t i = lake->pixels[index];
						if (i < bmpSize - width * 3 && provinceBuffer[ABOVE(i, width * 3)] == 254)
						{
							provinceBMP->setTriple(lakeColour, ABOVE(i, width * 3));
							lake->pixels.push_back(ABOVE(i, width * 3));
							newFound = true;
						}
						if (i > width * 3 && provinceBuffer[BELOW(i, width * 3)] == 254)
						{
							provinceBMP->setTriple(lakeColour, BELOW(i, width * 3));
							lake->pixels.push_back(BELOW(i, width * 3));
							newFound = true;
						}
						if (i + 3 < bmpSize - 3 && provinceBuffer[i + 3] == 254)
						{
							provinceBMP->setTriple(lakeColour, i + 3);
							lake->pixels.push_back((i + 3));
							newFound = true;
						}
						if (i - 3 > 0 && provinceBuffer[i - 3] == 254)
						{
							provinceBMP->setTriple(lakeColour, i - 3);
							lake->pixels.push_back((i - 3));
							newFound = true;
						}
					}
				}
			}
		}
		else {
			if (provinceBuffer[unassignedPixel] == 0)
			{
				uint32_t distance = UINT32_MAX;
				Prov* nextOwner = nullptr;
				for (Prov* P : provinces)
				{
					if ((P->colour.rgbtBlue == 1) /*|| (provinceBuffer[unassignedPixel] == 254 && P->colour.rgbtBlue == 255)*/) {
						//length of vector between current pixel and province pixel
						uint32_t x1 = 0;
						uint32_t y1 = 0;
						bool detailed = false;
						if (detailed) {
							uint32_t nearestPixelOfThatProvince = 0;
							uint32_t pixelDistance = INFINITY;
							for (int i = 0; i < (P->pixels.size()); i += 30)
							{
								if (i < P->pixels.size()) {
									uint32_t provincePixel = P->pixels[i];
									uint32_t x1temp = provincePixel % bmpWidth;
									uint32_t x2temp = unassignedPixel % bmpWidth;
									uint32_t y1temp = provincePixel / bmpHeight;
									uint32_t y2temp = unassignedPixel / bmpHeight;
									if (sqrt(((x1temp - x2temp) *(x1temp - x2temp)) + ((y1temp - y2temp) *(y1temp - y2temp))) < pixelDistance) {
										pixelDistance = (uint32_t)sqrt(((x1temp - x2temp) *(x1temp - x2temp)) + ((y1temp - y2temp) *(y1temp - y2temp)));
										nearestPixelOfThatProvince = provincePixel;
									}
								}
							}
							x1 = nearestPixelOfThatProvince / 3 % bmpWidth;
							uint32_t y1 = nearestPixelOfThatProvince / 3 / bmpHeight;
						}
						else {
							x1 = (P->center / 3) % bmpWidth;
							y1 = (P->center / 3) / bmpHeight;
						}
						uint32_t x2 = (unassignedPixel / 3) % bmpWidth;
						uint32_t y2 = (unassignedPixel / 3) / bmpHeight;
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
	for (auto prov : provinces)
	{
		prov->pixels.clear();
	}
	int r, g, b;
	for (uint32_t j = 0; j < provinceBMP->bitmapinfoheader.biSizeImage - 3; j += 3)
	{
		try {
			r = provinceBMP->getValueAt(j, 2);
			g = provinceBMP->getValueAt(j, 1);
			b = provinceBMP->getValueAt(j);
			provinceMap[r][g][b]->pixels.push_back(j);
		}
		catch (runtime_error e)
		{
			cout << "Runtime error " << r << " " << g << " " << b << endl;
		}
	}
}
//writes the continents to a bitmap, non-unique colours
void Terrain::prettyContinents(Bitmap * continentBMP)
{
	cout << "Creating continent" << endl;
	delete continentBMP->getBuffer();
	continentBMP->setBuffer(new BYTE[continentBMP->bitmapinfoheader.biSizeImage]);
	for (auto continent : continents) {
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
	cout << "Creating regions" << endl;
	regionBMP->setBuffer(new BYTE[regionBMP->bitmapinfoheader.biSizeImage]);
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
//evaluates province size to define wether it should be deleted in case it is too small
void Terrain::prettyProvinces(Bitmap * provinceBMP, uint32_t minProvSize, uint32_t updateThreshold)
{
	cout << "Beautifying provinces" << endl;
	for (auto province : provinces)
	{
		if (province->pixels.size() < minProvSize && !province->sea) {
			cout << "Eliminating small province" << endl;
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
	for (uint32_t i = 0; i < provinces.size(); i++)
	{
		if (provinces[i]->pixels.size() == 0 && !provinces[i]->sea) {
			provinces.erase(provinces.begin() + i);
			i--;
		}
	}
	vector<uint32_t> randomValuesCached;
	for (uint32_t i = 0; i < provinceBMP->bitmapinfoheader.biSizeImage / 64; i++) {
		randomValuesCached.push_back((*random)() % 4);
	}
	fill(provinceBMP, 1, 0, 0, provinceBMP->bitmapinfoheader.biSizeImage, std::ref(randomValuesCached), updateThreshold);
	assignRemainingPixels(provinceBMP, provinceBMP->getBuffer(), false);
	provPixels(provinceBMP);
}
//creates terrain around simplistic climate model
void Terrain::prettyTerrain(Bitmap * terrainBMP, Bitmap * heightmap, uint32_t seaLevel, uint32_t updateThreshold)
{
	cout << "Creating complex terrain" << endl;
	Visualizer::initializeWindow();
	//TODO CONFIG FOR ALL PARAMS
	uint32_t coastalDistanceInfluence = 30;
	uint32_t mountainStart = seaLevel + 40;
	uint32_t hillStart = seaLevel + 20;

	uint32_t arctic = 0;//0-3
	uint32_t arcticRange = 4;
	uint32_t plains = 5;
	uint32_t plainsRange = 4;
	uint32_t farmlands = 8;
	uint32_t farmlandsRange = 4;
	uint32_t forest = 12;
	uint32_t forestRange = 4;
	uint32_t hills = 16;
	uint32_t woods = 20;
	uint32_t mountains = 24; //24-31
	uint32_t plains2 = 32;
	uint32_t steppe = 36;
	uint32_t steppeRange = 4;
	uint32_t jungle = 40;
	uint32_t jungleRange = 4;
	uint32_t marsh = 44;
	uint32_t marshRange = 4;
	uint32_t desert = 48;
	uint32_t desertRange = 4;
	uint32_t coastal_desert = 52;
	uint32_t displayCounter = 0;

	for (auto prov : provinces)
	{
		displayCounter++;
		if (prov->sea)
			continue;
		vector<double> likelyHoods;
		double arcticLikelyhood = 0;
		double plainsLikelyhood = 0;
		double forestLikelyhood = 0;
		double farmlandsLikelyhood = 0;
		double steppeLikelyhood = 0;
		double jungleLikelyhood = 0;
		double marshLikelyhood = 0;
		double desertLikelyhood = 0;
		//determine north south temperature factor
		uint32_t equator = heightmap->bitmapinfoheader.biHeight / 2;
		uint32_t pixX = prov->center / 3 % (heightmap->bitmapinfoheader.biWidth);
		uint32_t pixY = (prov->center / 3 - pixX) / heightmap->bitmapinfoheader.biWidth;
		double temperatureFactor = (float)(pixY % equator) / (float)equator;


		//DISTANCE TO COAST
		float humidityFactor = 0;
		int offset = 0;
		int distanceNorth = 0;
		int distanceSouth = 0;
		int distanceEast = 0;
		int distanceWest = 0;
		uint32_t bitmapWidth = terrainBMP->bitmapinfoheader.biWidth;
		while (terrainBMP->getValueAt(prov->center / 3 + offset) != 254) {
			offset++;
			distanceEast = offset;
		}
		offset = 0;
		while (terrainBMP->getValueAt(prov->center / 3 - offset) != 254) {
			offset++;
			distanceWest = offset;
		}
		offset = bitmapWidth;
		while (terrainBMP->getValueAt(prov->center / 3 + offset) != 254) {
			offset += bitmapWidth;
			distanceNorth = offset / bitmapWidth;
		}
		offset = bitmapWidth;
		while (terrainBMP->getValueAt(prov->center / 3 - offset) != 254) {
			offset += bitmapWidth;
			distanceSouth = offset / bitmapWidth;
		}
		uint32_t distances[] = { (uint32_t)abs(distanceNorth), (uint32_t)abs(distanceEast),
								(uint32_t)abs(distanceWest), (uint32_t)abs(distanceSouth) };

		if (*std::min_element(distances, distances + 4) < coastalDistanceInfluence) {
			humidityFactor = (float)(1 - (*std::min_element(distances, distances + 4) / coastalDistanceInfluence));
		}
		//DISTANCE TO EQUATOR
		if (pixY >= equator) {
			temperatureFactor = 1 - temperatureFactor;
		}
		if (temperatureFactor < 0.1 && !prov->sea)//mountains
		{
			arcticLikelyhood += 1;
		}
		else if (temperatureFactor < 0.3 && temperatureFactor >= 0.1 && !prov->sea)
		{
			arcticLikelyhood += 0.3 - temperatureFactor;//0.2-0
			forestLikelyhood += 0.1 + temperatureFactor;//0.2-0.4
		}
		else if (temperatureFactor < 0.6 && temperatureFactor >= 0.3 && !prov->sea)
		{
			farmlandsLikelyhood += temperatureFactor - 0.3;//0.0-0.3
			marshLikelyhood += 0.03;
			forestLikelyhood += (0.7 - temperatureFactor);//0.4-0.1
		}
		else if (temperatureFactor < 0.75 && temperatureFactor >= 0.6 && !prov->sea)
		{
			farmlandsLikelyhood += 0.9 - temperatureFactor;//0.3-0.15
			steppeLikelyhood += temperatureFactor - 0.6;//0.0-0.15
		}
		else if (temperatureFactor < 0.85 && temperatureFactor >= 0.75 && !prov->sea)
		{
			steppeLikelyhood += humidityFactor * (temperatureFactor - 0.6);//0.15-0.25
			farmlandsLikelyhood += 0.5*humidityFactor;
			desertLikelyhood += 0.5*(1 - humidityFactor);//
		}
		else if (temperatureFactor >= 0.85 && !prov->sea)
		{
			steppeLikelyhood += (uint32_t)(1.0 - temperatureFactor);//0.25-0.1
			jungleLikelyhood += (temperatureFactor - 0.85) * 2;//0.0-0.7
		}



		//WESTWINDS




		likelyHoods.push_back(arcticLikelyhood);
		likelyHoods.push_back(plainsLikelyhood);
		likelyHoods.push_back(forestLikelyhood);
		likelyHoods.push_back(farmlandsLikelyhood);
		likelyHoods.push_back(steppeLikelyhood);
		likelyHoods.push_back(jungleLikelyhood);
		likelyHoods.push_back(marshLikelyhood);
		likelyHoods.push_back(desertLikelyhood);
		double sumOfAllLikelyhoods = 0;
		uint32_t highestLikelyhoodIndex = 0;
		for (uint32_t i = 0; i < likelyHoods.size(); i++) {
			sumOfAllLikelyhoods += likelyHoods[i];
		}

		vector<bool> candidates;
		for (double likelyHood : likelyHoods) {
			candidates.push_back(false);
		}

		for (auto candidate : candidates) {
			candidate = false;
		}
		bool foundAtLeastOne = false;
		while (!foundAtLeastOne) {
			for (uint32_t i = 0; i < likelyHoods.size(); i++) {
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
				terrainBMP->setSingle(pixel / 3, arctic + (*random)() % arcticRange);
				prov->climate = "inhospitable_climate";
				break;
			}
			case 1:
			{
				terrainBMP->setSingle(pixel / 3, plains + (*random)() % plainsRange);
				prov->climate = "temperate_climate";
				break;
			}
			case 2:
			{
				terrainBMP->setSingle(pixel / 3, forest + ((double)forestRange)*(1.0 - (temperatureFactor / 0.6)));
				prov->climate = "temperate_climate";
				if (temperatureFactor < 0.3)
					prov->climate = "harsh_climate";
				break;
			}
			case 3:
			{
				terrainBMP->setSingle(pixel / 3, farmlands + (*random)() % farmlandsRange);
				prov->climate = "mild_climate";
				break;
			}
			case 4:
			{
				terrainBMP->setSingle(pixel / 3, steppe + (*random)() % steppeRange);
				prov->climate = "harsh_climate";
				break;
			}
			case 5:
			{
				terrainBMP->setSingle(pixel / 3, jungle + (*random)() % jungleRange);
				prov->climate = "harsh_climate";
				break;
			}
			case 6:
			{
				terrainBMP->setSingle(pixel / 3, marsh + (*random)() % marshRange);
				prov->climate = "temperate_climate";
				break;
			}
			case 7:
			{
				terrainBMP->setSingle(pixel / 3, desert + (*random)() % desertRange);
				prov->climate = "inhospitable_climate";
				break;
			}
			}
			if (altitude > hillStart)//mountains
			{
				terrainBMP->setSingle(pixel / 3, (uint32_t)((float)(altitude - hillStart) / (float)(mountainStart - hillStart) *(float)4 + 16));
			}
			if (altitude > mountainStart)//mountains
			{
				if ((uint32_t)((float)(altitude - mountainStart) / (float)(210 - mountainStart) *(float)7 + 24) < 31) {
					terrainBMP->setSingle(pixel / 3, (uint32_t)((float)(altitude - mountainStart) / (float)(210 - mountainStart) *(float)7 + 24));
				}
				else { terrainBMP->setSingle(pixel / 3, 31); }
			}
		}
		if (displayCounter % updateThreshold == 0) {
			Visualizer::displayImage(terrainBMP->get24BitRepresentation());
		}
	}
	destroyAllWindows();
}
//creates rivers 
void Terrain::prettyRivers(Bitmap * riverBMP, Bitmap * heightmap, uint32_t riverAmount, uint32_t elevationTolerance, uint32_t seaLevel)
{
	cout << "Creating rivers" << endl;
	std::unordered_set<uint32_t> riverPixels;
	uint32_t maxRiverColour = 10;
	//TODO PARAMETRISATION
		//ELEVATION
		//COLOURRANGE
	uint32_t heightmapWidth = heightmap->bitmapinfoheader.biWidth;
	for (uint32_t i = 0; i < riverAmount; i++) {
		River* R = new River();
		this->rivers.push_back(R);

		//find random start point
		uint32_t start = 0;
		while (!(heightmap->getValueAt(start * 3) > seaLevel) && riverPixels.find(start) == riverPixels.end())
		{
			start = ((*random)() % heightmap->bitmapinfoheader.biSizeImage / 3);
		}
		start *= 3;
		R->setSource(start);
		R->setcurrentEnd(start);
		riverPixels.insert(R->getCurrentEnd());

		//check each direction for fastest decay in altitude
		vector<uint32_t> altitudes{ heightmap->getValueAt(ABOVE(R->getCurrentEnd(), heightmapWidth * 3 * 5)),
			heightmap->getValueAt(BELOW(R->getCurrentEnd(), heightmapWidth * 3 * 5)),
			heightmap->getValueAt(R->getCurrentEnd() - 15),
			heightmap->getValueAt(R->getCurrentEnd() + 15) };

		std::vector<uint32_t>::iterator result = std::min_element(std::begin(altitudes), std::end(altitudes));
		uint32_t favDirection = std::distance(std::begin(altitudes), result);
		//this variable is used to avoid rectangles in the river
		int previous = 0;
		while (heightmap->getValueAt(R->getCurrentEnd()) > seaLevel - 1) {
			uint32_t elevationToleranceOffset = 0;
			vector<uint32_t> candidates;
			//now expand to lower or equal pixel as long as possible
			while (elevationToleranceOffset < elevationTolerance &&candidates.size() == 0) {
				if (heightmap->getValueAt(ABOVE(R->getCurrentEnd(), heightmapWidth * 3)) < heightmap->getValueAt(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(ABOVE(R->getCurrentEnd(), heightmapWidth * 3)) && favDirection != 1) {
					if (ABOVE(R->getCurrentEnd(), heightmapWidth * 3) < heightmap->bitmapinfoheader.biSizeImage && previous != -(int)heightmapWidth * 3)
						candidates.push_back(ABOVE(R->getCurrentEnd(), heightmapWidth * 3));
				}
				if (heightmap->getValueAt(BELOW(R->getCurrentEnd(), heightmapWidth * 3)) < heightmap->getValueAt(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(BELOW(R->getCurrentEnd(), heightmapWidth * 3)) && favDirection != 0) {
					if (BELOW(R->getCurrentEnd(), heightmapWidth * 3) > heightmapWidth * 3 && previous != heightmapWidth * 3)
						candidates.push_back(BELOW(R->getCurrentEnd(), heightmapWidth * 3));
				}
				if (heightmap->getValueAt(LEFT(R->getCurrentEnd())) < heightmap->getValueAt(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(LEFT(R->getCurrentEnd())) && favDirection != 3) {
					if (LEFT(R->getCurrentEnd()) > 3 && previous != 3)
						candidates.push_back(LEFT(R->getCurrentEnd()));
				}
				if (heightmap->getValueAt(RIGHT(R->getCurrentEnd())) < heightmap->getValueAt(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(RIGHT(R->getCurrentEnd())) && favDirection != 2) {
					if (RIGHT(R->getCurrentEnd()) < heightmap->bitmapinfoheader.biSizeImage - 3 && previous != -3)
						candidates.push_back(RIGHT(R->getCurrentEnd()));
				}
				if (candidates.size() == 0) {
					elevationToleranceOffset++;
					if (elevationToleranceOffset >= elevationTolerance) {
						//cout << "RIVER stopped due to elevation with length: " << R->pixels.size() << endl;
						break;
					}
				}
			}
			if (candidates.size() == 0) {
				break;
			}
			uint32_t newPixel = candidates[(*random)() % candidates.size()];
			vector<uint32_t> directions;
			directions.push_back(LEFT(newPixel));
			directions.push_back(RIGHT(newPixel));
			directions.push_back(BELOW(newPixel, heightmapWidth * 3));
			directions.push_back(ABOVE(newPixel, heightmapWidth * 3));
			uint32_t neighbouringRiverPixels = 0;
			for (auto directionPixel : directions) {
				if (riverPixels.find(directionPixel) != riverPixels.end())
					neighbouringRiverPixels++;

			}
			if (neighbouringRiverPixels > 1) {
				//now save in which direction it went. Used again to avoid squares in river
				if (R->pixels.size() > 1)
					previous = (R->pixels[R->pixels.size() - 1]) - (R->pixels[R->pixels.size() - 2]);
				R->setcurrentEnd(newPixel);
				R->pixels.push_back(newPixel);
				riverPixels.insert(newPixel);
				for (auto river : rivers)
				{
					if (river->contains(newPixel))
					{
						river->addIngoing(R, newPixel);
					}
				}
				//cout << "RIVER ENDED IN OTHER river with length: " << R->pixels.size() << endl;
				break;
			}
			// if is already a river pixel, end this river
			if (riverPixels.find(newPixel) == riverPixels.end()) {
				//now save in which direction it went. Used again to avoid squares in river
				if (R->pixels.size() > 1)
					previous = (R->pixels[R->pixels.size() - 1]) - (R->pixels[R->pixels.size() - 2]);
				R->setcurrentEnd(newPixel);
				R->pixels.push_back(newPixel);
				riverPixels.insert(newPixel);
			}			//check if river borders other river

			else {
				for (auto river : rivers)
				{
					if (river->contains(newPixel))
					{
						river->addIngoing(R, newPixel);
					}
				}
				//cout << "RIVER ENDED IN OTHER river with length: " << R->pixels.size() << endl;
				break;
			}

			if (heightmap->getValueAt(R->getCurrentEnd()) <= seaLevel) {
				//cout << "RIVER ENDED IN SEA with length: " << R->pixels.size() << endl;
				break;
			}
		}
	}

	for (uint32_t i = 0; i < riverBMP->bitmapinfoheader.biSizeImage; i++)
	{
		if (heightmap->getValueAt(i * 3) > seaLevel)
			riverBMP->setSingle(i, 255);
		else
			riverBMP->setSingle(i, 254);
	}
	for (River* river : rivers) {
		if (river->pixels.size() < 10)
			continue;
		uint32_t riverColour = 2;
		riverBMP->setSingle(river->getSource() / 3, 0);
		for (uint32_t pix : river->pixels) {
			if (riverColour < maxRiverColour && river->getIngoingForKey(pix) != nullptr) {
				riverColour += river->getIngoingForKey(pix)->getIngoing().size() + 1;
				if (riverColour > maxRiverColour)
					riverColour = maxRiverColour;
			}
			riverBMP->setSingle(pix / 3, riverColour);
		}
	}


	for (int i = 0; i < provinces.size(); i++)
		provinces[i]->computeCandidates();
}
//creates the province map for fast access of provinces when only
//rgb values are available, removes need to search this province
boost::multi_array<Prov*, 3> Terrain::createProvinceMap()
{
	for (auto province : provinces) {
		provinceMap[province->colour.rgbtRed][province->colour.rgbtGreen][province->colour.rgbtBlue] = province;
	}
	return provinceMap;
}