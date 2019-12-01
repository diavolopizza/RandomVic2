#include "Terrain.h"
#include "../utils/BMPHandler.h"
#include "../FastNoise/FastNoise.h"
#include "boost/algorithm/clamp.hpp"
#include <windows.h>
#include <thread>


//MACROS
#define LEFT(val) \
(val-1)
#define RIGHT(val) \
(val+1)
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
	provinceMap.resize(256);
}

Terrain::~Terrain()
{

}

//creates the province map for fast access of provinces when only
//rgb values are available, removes need to search this province
MultiArray Terrain::createProvinceMap()
{
	for (auto province : provinces) {

		provinceMap.setValue(province->colour, province);
	}
	return provinceMap;
}
//
uint32_t Terrain::GetMinDistanceToProvince(uint32_t position, uint32_t width, uint32_t height) {
	uint32_t distance = MAXUINT32;
	for (Prov* P : provinces)
	{
		uint32_t x1 = P->center % width;
		uint32_t x2 = position % width;
		uint32_t y1 = P->center / height;
		uint32_t y2 = position / height;
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
	//if (end > width * height )
	//	end = width * height ;
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
			if (RGBBMP->getValueAtXYPosition(heightIndex, widthIndex) < seaLevel) {
				//inRangePixels.push_back((heightIndex * widthIndex + widthIndex) );
				index++;
			}
		}
	}

	//for (int i = start; i < end; i += 3)
	//{
	//	uint32_t x1 = i  % width;
	//	uint32_t x2 = position  % width;
	//	uint32_t y1 = i  / height;
	//	uint32_t y2 = position  / height;
	//	distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
	//	if (distance < radius && RGBBMP->getValueAt(i) < 90) {
	//		inRangePixels[index] = i;
	//		index++;
	//	}
	//}
	return index;
}
//Utility to find starting point of new province
void Terrain::determineStartingPixel(Bitmap* bitmap, vector<uint32_t> &provincePixels, RGBTRIPLE &provinceColour, uint32_t provinceSize) {
	uint32_t bmpWidth = bitmap->bInfoHeader.biWidth;
	uint32_t bmpHeight = bitmap->bInfoHeader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	uint32_t startingPixel = (*random)() % bmpSize;//startingpixel is anywhere in the file
	while (!(startingPixel >= bmpWidth && startingPixel <= bmpSize - bmpWidth && bitmap->getValueAtIndex(startingPixel) == provinceColour.rgbtBlue - 1)
		&& !GetMinDistanceToProvince(startingPixel, bmpWidth, bmpHeight) < 3 * sqrt(provinceSize / (std::atan(1) * 4)))
	{
		startingPixel = (*random)() % bmpSize; //startingpixel is anywhere in the file
	}
	bitmap->setTripleAtIndex(provinceColour, startingPixel);
	provincePixels.push_back(startingPixel);
}
//evaluate if province is coastal
void Terrain::evaluateCoasts(Bitmap * provinceBMP)
{
	for (auto prov : provinces) {
		if (!prov->sea) {
			for (auto pixel : prov->pixels) {
				if (provinceBMP->getValueAtIndex(LEFT(pixel) == 254)) {
					prov->coastal = true;
					continue;
				}
				else if (provinceBMP->getValueAtIndex(RIGHT(pixel) == 254)) {
					prov->coastal = true;
					continue;
				}
				else if (provinceBMP->getValueAtIndex(BELOW(pixel, provinceBMP->bInfoHeader.biWidth) == 254)) {
					prov->coastal = true;
					continue;
				}
				else if (provinceBMP->getValueAtIndex(ABOVE(pixel, provinceBMP->bInfoHeader.biWidth) == 254)) {
					prov->coastal = true;
				}
			}
		}
	}
}
//Finds neighbours of all provinces and assigns them
void Terrain::evaluateNeighbours(Bitmap * provinceBMP)
{
	uint32_t width = provinceBMP->bInfoHeader.biWidth;
	for (uint32_t i = 0; i < provinceBMP->bInfoHeader.biWidth * provinceBMP->bInfoHeader.biHeight - width; i++)
	{
		Prov* left = provinceMap[provinceBMP->getTripleAtIndex(i)];
		Prov* right = provinceMap[provinceBMP->getTripleAtIndex(i + 1)];
		Prov* below = provinceMap[provinceBMP->getTripleAtIndex(i + width)];
		if (left == nullptr || right == nullptr)
			return;
		if (!(left == right)) {
			left->setNeighbour(right, true);
		}
		else if (!(left == below)) {
			left->setNeighbour(below, true);
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
	RGBTRIPLE colour;
	for (uint32_t j = 0; j < provinceBMP->bInfoHeader.biSizeImage / 3 - 1; j++)
	{
		try {
			colour = provinceBMP->getTripleAtIndex(j);
			provinceMap[colour]->pixels.push_back(j);
		}
		catch (runtime_error e)
		{
			cout << "Accessing province map with r,g,b: " << colour.rgbtRed << " " << colour.rgbtGreen << " " << colour.rgbtBlue << " results in undefined value error" << endl;
		}
	}
}
//creates the heightmap with a given seed
BYTE* Terrain::heightMap(Bitmap * RGBBMP, uint32_t seed)
{
	if (Data::getInstance().opencvVisualisation)
		Visualizer::initializeWindow();
	char redo;
	/*do {*/
	cout << "Creating Heightmap" << endl;
	FastNoise myNoise; // Create a FastNoise object
	myNoise.SetNoiseType(FastNoise::NoiseType::SimplexFractal); // Set the desired noise type
	myNoise.SetSeed(seed);
	myNoise.SetFrequency(Data::getInstance().fractalFrequency);
	myNoise.SetFractalOctaves(Data::getInstance().fractalOctaves);
	myNoise.SetFractalGain(Data::getInstance().fractalGain);
	myNoise.SetFractalType(FastNoise::FractalType::FBM);

	FastNoise myNoise2; // Create a FastNoise object
	myNoise2.SetNoiseType(FastNoise::NoiseType::SimplexFractal); // Set the desired noise type
	myNoise.SetSeed(seed + 1337);
	//myNoise2.SetSeed(3);
	myNoise2.SetFrequency(0.0018f);
	myNoise2.SetFractalOctaves(Data::getInstance().fractalOctaves);
	myNoise2.SetFractalGain(Data::getInstance().fractalGain);
	myNoise2.SetFractalType(FastNoise::FBM);

	const uint32_t width = RGBBMP->bInfoHeader.biWidth;
	const uint32_t height = RGBBMP->bInfoHeader.biHeight;
	uint32_t delimiter = width / Data::getInstance().divideThreshold;
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
			uint32_t completeNoise;
			if (noiseLevel >= Data::getInstance().seaLevel && Data::getInstance().complexHeight) {
				FN_DECIMAL noiseLevel2 = (myNoise2.GetNoise(x, y) + 1) * 128 * factor;
				completeNoise = (noiseLevel + (noiseLevel2 * 0.5)) *0.67;
				if (completeNoise < Data::getInstance().seaLevel + 1) {
					completeNoise = Data::getInstance().seaLevel + 1;/*
					completeNoise /= 4;
					completeNoise *= 4;*/
				}
			}
			else {
				completeNoise = noiseLevel;/*
				completeNoise /= 4;
				completeNoise *= 4;*/
			}
			RGBTRIPLE colour{ 1 + completeNoise, 1 + completeNoise, 1 + completeNoise };
			RGBBMP->setTripleAtXYPosition(colour, x, y);
		}
		if (x % Data::getInstance().updateThreshold == 0 && Data::getInstance().opencvVisualisation)
			Visualizer::displayImage(RGBBMP);
	}
	/*'n' >> redo;*/
//} /*while (redo == 'y');*/
	cvDestroyAllWindows();
	return RGBBMP->getBuffer();
}
//creates the terrain, factoring in heightmap
void Terrain::createTerrain(Bitmap * terrainBMP, Bitmap* heightMapBmp)
{
	double tempLandPercentage = 0;
	uint32_t landPixels = 0;
	while (0.05 < fabs(tempLandPercentage - (double)Data::getInstance().landMassPercentage / 100.0)) {
		if(tempLandPercentage - (double)Data::getInstance().landMassPercentage / 100.0 < 0)
			Data::getInstance().seaLevel--;
		else
			Data::getInstance().seaLevel++;

		cout << "Creating basic terrain from heightmap" << endl;
		uint32_t width = terrainBMP->bInfoHeader.biWidth;
		uint32_t height = terrainBMP->bInfoHeader.biHeight;

		for (uint32_t x = 0; x < height; x++)
		{
			for (uint32_t y = 0; y < width; y++)
			{
				if (heightMapBmp->getValueAtXYPosition(x, y) > Data::getInstance().seaLevel) {
					terrainBMP->setValueAtXYPosition(13, x, y);
					landPixels++;
				}
				else {
					terrainBMP->setValueAtXYPosition(254, x, y);
				}
			}
		}
		tempLandPercentage = (double)landPixels / (double)(terrainBMP->bInfoHeader.biSizeImage);
		cout << "Landpercentage: " << tempLandPercentage << endl;
		landPixels = 0;
	}
}
//generates all land provinces
BYTE* Terrain::landProvinces(uint32_t numoflandprov, Bitmap * terrainBMP, Bitmap * provinceBMP, Bitmap* riverBMP, uint32_t updateThreshold)
{
	cout << "Generating landprovinces" << endl;
	uint32_t bmpWidth = terrainBMP->bInfoHeader.biWidth;
	uint32_t bmpHeight = terrainBMP->bInfoHeader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	RGBTRIPLE rgbHigh{ 254, 254, 254 };
	RGBTRIPLE rgbLow{ 0, 0, 0 };
	//initialize buffer
	BYTE* provinceBuffer = provinceBMP->getBuffer();
	for (uint32_t i = 0; i < bmpSize; i++) {
		if (terrainBMP->getValueAtIndex(i) == 254) {
			//sea
			provinceBMP->setTripleAtIndex(rgbHigh, i);
		}
		else {
			//land
			provinceBMP->setTripleAtIndex(rgbLow, i);
		}
	}
	//assign province size
	uint32_t provincesize = (int)((float)bmpSize * 0.6f / (float)numoflandprov);//better calculation?
	//assign pixels to this new province
	provinceCreation(provinceBMP, provincesize, numoflandprov, 0, 0);
	//For multithreading: create vector of random values. Used for performance improvements, as ranlux48 is using locks, and new instances would remove determination.
	vector<uint32_t> randomValuesCached;
	for (uint32_t i = 0; i < bmpSize / 64; i++) {
		randomValuesCached.push_back((*random)() % 4);
	}
	uint32_t threadCount = 1;
	//decrement number of threads, until biSizeImage can be divided by threadCount without any rest
	while (bmpSize % threadCount != 0)
	{
		threadCount--;
	}
	const uint32_t numThreads = threadCount;
	std::vector<std::thread> threads;
	for (uint32_t i = 0; i < numThreads; ++i) {
		uint32_t from = i * (bmpSize / numThreads);
		uint32_t to = (i + 1) * (bmpSize / numThreads);
		threads.push_back(std::thread(&Terrain::fill, this, std::ref(provinceBMP), std::ref(riverBMP), (uint32_t)1, (uint32_t)0, from, to, std::ref(randomValuesCached), updateThreshold));
	}
	//wait for threads to finish
	for (auto& t : threads) {
		t.join();
	}
	assignRemainingPixels(provinceBMP, false);
	return provinceBuffer;
}
//generates all land provinces
BYTE* Terrain::seaProvinces(uint32_t numOfSeaProv, uint32_t numoflandprov, Bitmap * terrainBMP, Bitmap * provinceBMP, Bitmap* riverBMP, uint32_t updateThreshold)
{
	uint32_t bmpWidth = terrainBMP->bInfoHeader.biWidth;
	uint32_t bmpHeight = terrainBMP->bInfoHeader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;
	BYTE* provinceBuffer = provinceBMP->getBuffer();
	cout << "Generating seaprovinces" << endl;
	uint32_t provincesize = bmpSize / numOfSeaProv;//better calculation?
	provinceCreation(provinceBMP, provincesize, numOfSeaProv, numoflandprov, 254);
	//multithreading
	uint32_t threadCount = 1;
	vector<uint32_t> randomValuesCached;
	for (uint32_t i = 0; i < provinceBMP->bInfoHeader.biSizeImage / 64; i++) {
		randomValuesCached.push_back((*random)() % 4);
	}
	while (provinceBMP->bInfoHeader.biSizeImage % threadCount != 0)
	{
		threadCount--;
	}
	const uint32_t numThreads = threadCount;
	std::vector<std::thread> threads;
	for (uint32_t i = 0; i < numThreads; ++i) {
		uint32_t from = i * (bmpSize / numThreads);
		uint32_t to = (i + 1) * (bmpSize / numThreads);
		threads.push_back(std::thread(&Terrain::fill, this, std::ref(provinceBMP), std::ref(riverBMP), (uint32_t)255, (uint32_t)254, from, to, std::ref(randomValuesCached), updateThreshold));
	}
	//wait for threads to finish
	for (auto& t : threads) {
		t.join();
	}
	assignRemainingPixels(provinceBMP, true);
	return provinceBuffer;
}
//creates the basic province with a random shape
void Terrain::provinceCreation(Bitmap * provinceBMP, uint32_t provinceSize, uint32_t numOfLandProv, uint32_t offset, uint32_t greyval)
{
	uint32_t bmpWidth = provinceBMP->bInfoHeader.biWidth;
	uint32_t bmpHeight = provinceBMP->bInfoHeader.biHeight;
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
		for (uint32_t x = 0; x < 2 - 1; x++)
		{
			uint32_t currentPixel = 0;
			while (currentPixel <= bmpWidth || currentPixel >= bmpSize - bmpWidth)
			{
				currentPixel = P->pixels[(*random)() % P->pixels.size()];
			}
			if (provinceBMP->getValueAtIndex(RIGHT(currentPixel)) == greyval)
			{
				if ((currentPixel + 1) % (bmpWidth / 2) != 0) {
					provinceBMP->setTripleAtIndex(provinceColour, RIGHT(currentPixel));
					P->pixels.push_back(RIGHT(currentPixel));
					x++;
				}
			}
			if (provinceBMP->getValueAtIndex(LEFT(currentPixel)) == greyval)
			{
				if ((currentPixel) % (bmpWidth / 2) != 0) {
					provinceBMP->setTripleAtIndex(provinceColour, LEFT(currentPixel));
					P->pixels.push_back(LEFT(currentPixel));
					x++;
				}
			}
			if (provinceBMP->getValueAtIndex(ABOVE(currentPixel, bmpWidth)) == greyval)
			{
				provinceBMP->setTripleAtIndex(provinceColour, ABOVE(currentPixel, bmpWidth));
				P->pixels.push_back(ABOVE(currentPixel, bmpWidth));
				x++;
			}
			if (provinceBMP->getValueAtIndex(BELOW(currentPixel, bmpWidth)) == greyval)
			{
				provinceBMP->setTripleAtIndex(provinceColour, BELOW(currentPixel, bmpWidth));
				P->pixels.push_back(BELOW(currentPixel, bmpWidth));
				x++;
			}
		}
		P->center = P->pixels[0];
		provinces.push_back(P);
	}
}
//fills unassigned pixels in iterations, so provinces grow
void Terrain::fill(Bitmap* provinceBMP, Bitmap* riverBMP, uint32_t greyVal, uint32_t fillVal, uint32_t from, uint32_t to, vector<uint32_t> &randomValuesCached, uint32_t updateThreshold)
{
	if (Data::getInstance().opencvVisualisation)
		Visualizer::initializeWindow();
	cout << "Starting filling of unassigned pixels from pixel " << from << " to pixel " << to << endl;
	uint32_t bmpWidth = provinceBMP->bInfoHeader.biWidth;
	uint32_t bmpHeight = provinceBMP->bInfoHeader.biHeight;
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
		for (uint32_t unassignedPixel = from; unassignedPixel < to; unassignedPixel++)
		{
			if (provinceBMP->getValueAtIndex(unassignedPixel) == fillVal)
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
					if (unassignedPixel < bmpSize - 1 && provinceBMP->getValueAtIndex(RIGHT(unassignedPixel)) != 0 && provinceBMP->getValueAtIndex(RIGHT(unassignedPixel)) == greyVal)
					{
						//TODO: add river crossings to each province
						if ((unassignedPixel + 1) % (bmpWidth) != 0 && !(riverBMP->getValueAtIndex(RIGHT(unassignedPixel)) <= 10))
							provinceBMP->copyTripleToIndex(unassignedPixel, RIGHT(unassignedPixel));
					}
					break;
				}
				case 1:
				{
					if (unassignedPixel > 1 && provinceBMP->getValueAtIndex(LEFT(unassignedPixel)) != 0 && provinceBMP->getValueAtIndex(LEFT(unassignedPixel)) == greyVal)
					{
						if (unassignedPixel % (bmpWidth) != 0 && !(riverBMP->getValueAtIndex(LEFT(unassignedPixel)) <= 10))
							provinceBMP->copyTripleToIndex(unassignedPixel, LEFT(unassignedPixel));
					}
					break;
				}
				case 2:
				{
					if (unassignedPixel < bmpSize - bmpWidth && provinceBMP->getValueAtIndex(ABOVE(unassignedPixel, bmpWidth)) != 0 && provinceBMP->getValueAtIndex(ABOVE(unassignedPixel, bmpWidth)) == greyVal)
					{
						if (!(riverBMP->getValueAtIndex(ABOVE(unassignedPixel, bmpWidth)) <= 10))
							provinceBMP->copyTripleToIndex(unassignedPixel, ABOVE(unassignedPixel, bmpWidth));
					}
					break;
				}
				case 3:
				{
					if (unassignedPixel > bmpWidth && provinceBMP->getValueAtIndex(BELOW(unassignedPixel, bmpWidth)) != 0 && provinceBMP->getValueAtIndex(BELOW(unassignedPixel, bmpWidth)) == greyVal)
					{
						if (!(riverBMP->getValueAtIndex(BELOW(unassignedPixel, bmpWidth)) <= 10))
							provinceBMP->copyTripleToIndex(unassignedPixel, BELOW(unassignedPixel, bmpWidth));
					}
					break;
				}
				}
			}
		}
		if (unassignedPixels < imageDrawn - updateThreshold * 50 && Data::getInstance().opencvVisualisation) {
			Visualizer::displayImage(provinceBMP);
			imageDrawn = unassignedPixels;
		}
	}
	cvDestroyAllWindows();
}
//evaluates province size to define wether it should be deleted in case it is too small
void Terrain::prettyProvinces(Bitmap * provinceBMP, Bitmap* riverBMP, uint32_t minProvSize)
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
				provinceBMP->setTripleAtIndex(colour, pixel);
			}
			province->pixels.clear();
			provinceMap.setValue(province->colour, nullptr);
		}
	}
	for (uint32_t i = 0; i < provinces.size(); i++)
	{
		if (provinces[i]->pixels.size() == 0 && !provinces[i]->sea) {

			provinces.erase(provinces.begin() + i);
			i--;
			for (int u = i; u < provinces.size(); u++)
			{
				//fix province IDs after deletion
				provinces[u]->provID--;
			}
		}
	}
	vector<uint32_t> randomValuesCached;
	for (uint32_t i = 0; i < provinceBMP->bInfoHeader.biSizeImage / 64; i++) {
		randomValuesCached.push_back((*random)() % 4);
	}
	fill(provinceBMP, std::ref(riverBMP), 1, 0, 0, provinceBMP->bInfoHeader.biSizeImage / 3, std::ref(randomValuesCached), 200);
	assignRemainingPixels(provinceBMP, false);
	provPixels(provinceBMP);
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
		if (regions[i]->provinces.size() < minProvPerRegion - 1) {
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
					uint32_t x1 = P->center  % width;
					uint32_t x2 = prov->center  % width;
					uint32_t y1 = P->center / height;
					uint32_t y2 = prov->center / height;
					if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
						distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
						nextOwner = P->region;
					}
				}
			}
			//if (nextOwner != nullptr)
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
					region->setNeighbour(prov2->region, true);
				}
			}
		}
	}

}
//writes the regions to a bitmap, non-unique colours
void Terrain::prettyRegions(Bitmap * regionBMP)
{
	cout << "Creating regions" << endl;
	regionBMP->setBuffer(new BYTE[regionBMP->bInfoHeader.biSizeImage]);
	for (auto region : regions) {
		RGBTRIPLE regionColour;
		regionColour.rgbtBlue = (*random)() % 256;
		regionColour.rgbtGreen = (*random)() % 256;
		regionColour.rgbtRed = (*random)() % 256;
		for (auto province : region->provinces)
		{
			for (int pixel : province->pixels)
			{
				regionBMP->setTripleAtIndex(regionColour, pixel);
			}
		}
	}
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
					uint32_t x1 = P->center  % width;
					uint32_t x2 = prov->center  % width;
					uint32_t y1 = P->center / height;
					uint32_t y2 = prov->center / height;
					if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
						distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
						nextOwner = P->continent;
					}
				}
			}
			//if (nextOwner != nullptr)
			prov->assignContinent(nextOwner);
		}
	}
}
//writes the continents to a bitmap, non-unique colours
void Terrain::prettyContinents(Bitmap * continentBMP)
{
	cout << "Creating continent" << endl;
	delete continentBMP->getBuffer();
	continentBMP->setBuffer(new BYTE[continentBMP->bInfoHeader.biSizeImage]);
	for (auto continent : continents) {
		RGBTRIPLE continentColour;
		continentColour.rgbtBlue = (*random)() % 256;
		continentColour.rgbtGreen = (*random)() % 256;
		continentColour.rgbtRed = (*random)() % 256;

		for (auto province : continent->provinces)
		{
			for (uint32_t pixel : province->pixels)
			{
				continentBMP->setTripleAtIndex(continentColour, pixel);
			}
		}

	}
}
double calcMountainShadowAridity(Bitmap * heightmapBMP, uint32_t heightPos, uint32_t widthPos, int currentDirection, uint32_t seaLevel, double windIntensity) {
	uint32_t width = heightmapBMP->bInfoHeader.biWidth;
	//in regions with low windintensity, this effect has a lower range
	uint32_t maxEffectDistance = ((double)width / 100.0) * windIntensity;
	uint32_t mountainPixelsInRange = 0;
	for (int i = 0; i < maxEffectDistance; i++)
	{
		//TODO
		//if (heightmapBMP->getValueAtIndex((heightPos * width + widthPos + (currentDirection * i)) ) > seaLevel * 1.4)
		//{
		//	mountainPixelsInRange++;
		//}
	}
	return windIntensity * ((double)mountainPixelsInRange / (double)maxEffectDistance);
}
double calcCoastalHumidity(Bitmap * heightmapBMP, uint32_t heightPos, uint32_t widthPos, int windDirection, uint32_t seaLevel, double windIntensity, uint32_t width, uint32_t height) {
	//the more continental, the less of an influence the distance to a coast has
	uint32_t continentality = 0;
	//East/west directions are more important that north/south, as important winds travel east/west more often
	uint32_t maxEffectDistance = width / 200;
	//in the opposite direction of the major global winds, the distance to look at is much larger
	uint32_t windFactor = 20 * windIntensity;
	uint32_t coastDistance = maxEffectDistance * windFactor;
	uint32_t windDistance = maxEffectDistance * windFactor;

	//the direction opposite to the winds, e.g. west in case of west winds(winds coming from the west)
	for (int i = 0; i < maxEffectDistance * windFactor; i++)
	{
		int value = heightmapBMP->getValueAtXYPosition(heightPos, widthPos + (i * windDirection));
		if (value != -1 && value < seaLevel)
		{
			if (i < windDistance)
				windDistance = i;
		}
	}
	for (int x = 0; x < maxEffectDistance * windFactor; x++) {
		int value = heightmapBMP->getValueAtXYPosition(heightPos, widthPos + ((windDistance + x) * windDirection));
		if (value != -1 && value > seaLevel) {
			continentality++;
		}
	}
	//the direction opposite to the winds, e.g. west in case of west winds(winds coming from the west)
	for (int i = 0; i < maxEffectDistance; i++)
	{
		int value = heightmapBMP->getValueAtXYPosition(heightPos, widthPos + (i * windDirection));
		if (value != -1 && value < seaLevel)
		{
			if (i < coastDistance)
				coastDistance = i;
		}

	}
	//the direction the winds are going(e.g. east in case of west winds)
	for (int i = 0; i < maxEffectDistance; i++)
	{
		int value = heightmapBMP->getValueAtXYPosition(heightPos, widthPos + (i * windDirection * -1));
		if (value != -1 && value < seaLevel)
		{
			if (i < coastDistance)
				coastDistance = i;
		}
	}
	//north
	for (int i = 0; i < maxEffectDistance; i++)
	{
		int value = heightmapBMP->getValueAtXYPosition(heightPos + i, widthPos);
		if (value != -1 && value < seaLevel)
		{
			if (i < coastDistance)
				coastDistance = i;
		}
	}
	//south
	for (int i = 0; i < maxEffectDistance; i++)
	{
		int value = heightmapBMP->getValueAtXYPosition(heightPos - i, widthPos);
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
	if (Data::getInstance().opencvVisualisation)
		Visualizer::initializeWindow();
	cout << "Creating humidity map" << endl;
	double polarEasterlies = 0.3;
	double westerlies = 0.7;
	double tradeWinds = 1;
	const uint32_t width = heightmapBMP->bInfoHeader.biWidth;
	const uint32_t height = heightmapBMP->bInfoHeader.biHeight;

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
			//TODO
			if (heightmapBMP->getValueAtIndex((x * width + y)) > seaLevel)
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
					if (abs(heightf - tradeWinds) < 0.3) // near equator
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
					humidityBMP->setTripleAtIndex(colour, (x * width + y));
				}
			}
		}
		if (x % updateThreshold == 0 && Data::getInstance().opencvVisualisation)
			Visualizer::displayImage(humidityBMP);
	}

	/*uint32_t smoothDistance = 2;
	for (int i = width  * smoothDistance + smoothDistance; i < humidityBMP->bInfoHeader.biSizeImage - width  * smoothDistance - smoothDistance; i += 3)
	{
		RGBTRIPLE north = humidityBMP->getTripleAtIndex(i - width  * smoothDistance);
		RGBTRIPLE northwest = humidityBMP->getTripleAtIndex(i - width  * smoothDistance - smoothDistance );
		RGBTRIPLE northeast = humidityBMP->getTripleAtIndex(i - width  * smoothDistance + smoothDistance );
		RGBTRIPLE south = humidityBMP->getTripleAtIndex(i + width  * smoothDistance);
		RGBTRIPLE southwest = humidityBMP->getTripleAtIndex(i + width  * smoothDistance - smoothDistance );
		RGBTRIPLE southeast = humidityBMP->getTripleAtIndex(i + width  * smoothDistance + smoothDistance );
		RGBTRIPLE west = humidityBMP->getTripleAtIndex(i + smoothDistance );
		RGBTRIPLE east = humidityBMP->getTripleAtIndex(i - smoothDistance );
		RGBTRIPLE colour;
		colour.rgbtBlue = (double)((int)north.rgbtBlue + (int)northwest.rgbtBlue + (int)northeast.rgbtBlue + (int)southwest.rgbtBlue + (int)southeast.rgbtBlue + (int)south.rgbtBlue + (int)west.rgbtBlue + (int)east.rgbtBlue) / 8;
		colour.rgbtGreen = (double)((int)north.rgbtGreen + (int)northwest.rgbtGreen + (int)northeast.rgbtGreen + (int)southwest.rgbtGreen + (int)southeast.rgbtGreen + (int)south.rgbtGreen + (int)west.rgbtGreen + (int)east.rgbtGreen) / 8;
		colour.rgbtRed = (double)((int)north.rgbtRed + (int)northwest.rgbtRed + (int)northeast.rgbtRed + (int)southwest.rgbtRed + (int)southeast.rgbtRed + (int)south.rgbtRed + (int)west.rgbtRed + (int)east.rgbtRed) / 8;

		humidityBMP->setTripleAtIndex(colour, i);
	}*/
	return humidityBMP->getBuffer();
}
//assigns all unassigned pixels to the nearest province
void Terrain::assignRemainingPixels(Bitmap * provinceBMP, bool sea) {
	cout << "Assigning remaining pixels provinces" << endl;
	//Assign remaining pixels
	uint32_t bmpWidth = provinceBMP->bInfoHeader.biWidth;
	uint32_t bmpHeight = provinceBMP->bInfoHeader.biHeight;
	uint32_t bmpSize = bmpWidth * bmpHeight;

	for (uint32_t unassignedPixel = 0; unassignedPixel < bmpSize; unassignedPixel++)
	{
		if (sea) {
			if (provinceBMP->getValueAtIndex(unassignedPixel) == 254)
			{
				//find unique colour
				RGBTRIPLE lakeColour;
				bool unique = false;
				while (!unique) {
					lakeColour.rgbtBlue = 204;
					lakeColour.rgbtGreen = 1 + (*random)() % 255;
					lakeColour.rgbtRed = 1 + (*random)() % 255;
					if (provinceMap[lakeColour] == nullptr)
					{
						unique = true;
					}
				}
				Prov * lake = new Prov(provinces.size() + 1, lakeColour, true, this->random);
				provinceMap.setValue(lakeColour, lake);
				provinces.push_back(lake);
				lake->pixels.push_back(unassignedPixel);
				lake->center = unassignedPixel;
				provinceBMP->setTripleAtIndex(lakeColour, unassignedPixel);
				bool newFound = false;
				bool first = true;
				while (newFound || first) {
					newFound = false;
					first = false;
					for (uint32_t index = 0; index < lake->pixels.size(); index++) {
						uint32_t i = lake->pixels[index];
						if (i < bmpSize - bmpWidth && provinceBMP->getValueAtIndex(ABOVE(i, bmpWidth)) == 254)
						{
							provinceBMP->setTripleAtIndex(lakeColour, ABOVE(i, bmpWidth));
							lake->pixels.push_back(ABOVE(i, bmpWidth));
							newFound = true;
						}
						if (i > bmpWidth  && provinceBMP->getValueAtIndex(BELOW(i, bmpWidth)) == 254)
						{
							provinceBMP->setTripleAtIndex(lakeColour, BELOW(i, bmpWidth));
							lake->pixels.push_back(BELOW(i, bmpWidth));
							newFound = true;
						}
						if (i + 1 < bmpSize - 1 && provinceBMP->getValueAtIndex(i + 1) == 254)
						{
							provinceBMP->setTripleAtIndex(lakeColour, i + 1);
							lake->pixels.push_back((i + 1));
							newFound = true;
						}
						if (i - 1 > 0 && provinceBMP->getValueAtIndex(i - 1) == 254)
						{
							provinceBMP->setTripleAtIndex(lakeColour, i - 1);
							lake->pixels.push_back((i - 1));
							newFound = true;
						}
					}
				}
			}
		}
		else {
			if (provinceBMP->getValueAtIndex(unassignedPixel) == 0)
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
							x1 = nearestPixelOfThatProvince % bmpWidth;
							uint32_t y1 = nearestPixelOfThatProvince / bmpHeight;
						}
						else {
							x1 = (P->center) % bmpWidth;
							y1 = (P->center) / bmpHeight;
						}
						uint32_t x2 = (unassignedPixel) % bmpWidth;
						uint32_t y2 = (unassignedPixel) / bmpHeight;
						if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
							distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
							nextOwner = P;
						}
					}
				}
				provinceBMP->setTripleAtIndex(nextOwner->colour, unassignedPixel);
			}
		}
	}
}
//creates terrain around simplistic climate model
void Terrain::prettyTerrain(Bitmap * terrainBMP, Bitmap * heightmap, uint32_t seaLevel, uint32_t updateThreshold)
{
	cout << "Creating complex terrain" << endl;
	if (Data::getInstance().opencvVisualisation)
		Visualizer::initializeWindow();
	//TODO CONFIG FOR ALL PARAMS
	uint32_t coastalDistanceInfluence = 30;
	uint32_t mountainStart = seaLevel + (uint32_t)((float)seaLevel * 0.4);
	uint32_t hillStart = seaLevel + (uint32_t)((float)seaLevel * 0.2);

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
		uint32_t equator = heightmap->bInfoHeader.biHeight / 2;
		uint32_t pixX = prov->center % (heightmap->bInfoHeader.biWidth);
		uint32_t pixY = (prov->center - pixX) / heightmap->bInfoHeader.biWidth;
		double temperatureFactor = (float)(pixY % equator) / (float)equator;


		//DISTANCE TO COAST
		float humidityFactor = 0;
		int offset = 0;
		int distanceNorth = 0;
		int distanceSouth = 0;
		int distanceEast = 0;
		int distanceWest = 0;
		uint32_t bitmapWidth = terrainBMP->bInfoHeader.biWidth;
		//TODO
		while (terrainBMP->getValueAtIndex(prov->center + offset) != 254) {
			offset++;
			distanceEast = offset;
		}
		offset = 0;
		while (terrainBMP->getValueAtIndex(prov->center - offset) != 254) {
			offset++;
			distanceWest = offset;
		}
		offset = bitmapWidth;
		while (terrainBMP->getValueAtIndex(prov->center + offset) != 254) {
			offset += bitmapWidth;
			distanceNorth = offset / bitmapWidth;
		}
		offset = bitmapWidth;
		while (terrainBMP->getValueAtIndex(prov->center - offset) != 254) {
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
			uint32_t altitude = heightmap->getValueAtIndex(pixel);
			switch (index) {
			case 0:
			{
				terrainBMP->setValueAtIndex(pixel, arctic + (*random)() % arcticRange);
				prov->climate = "inhospitable_climate";
				break;
			}
			case 1:
			{
				terrainBMP->setValueAtIndex(pixel, plains + (*random)() % plainsRange);
				prov->climate = "temperate_climate";
				break;
			}
			case 2:
			{
				terrainBMP->setValueAtIndex(pixel, forest + ((double)forestRange)*(1.0 - (temperatureFactor / 0.6)));
				prov->climate = "temperate_climate";
				if (temperatureFactor < 0.3)
					prov->climate = "harsh_climate";
				break;
			}
			case 3:
			{
				terrainBMP->setValueAtIndex(pixel, farmlands + (*random)() % farmlandsRange);
				prov->climate = "mild_climate";
				break;
			}
			case 4:
			{
				terrainBMP->setValueAtIndex(pixel, steppe + (*random)() % steppeRange);
				prov->climate = "harsh_climate";
				break;
			}
			case 5:
			{
				terrainBMP->setValueAtIndex(pixel, jungle + (*random)() % jungleRange);
				prov->climate = "harsh_climate";
				break;
			}
			case 6:
			{
				terrainBMP->setValueAtIndex(pixel, marsh + (*random)() % marshRange);
				prov->climate = "temperate_climate";
				break;
			}
			case 7:
			{
				terrainBMP->setValueAtIndex(pixel, desert + (*random)() % desertRange);
				prov->climate = "inhospitable_climate";
				break;
			}
			}
			if (altitude > hillStart)//mountains
			{
				terrainBMP->setValueAtIndex(pixel, (uint32_t)((float)(altitude - hillStart) / (float)(mountainStart - hillStart) *(float)4 + 16));
			}
			if (altitude > mountainStart)//mountains
			{
				if ((uint32_t)((float)(altitude - mountainStart) / (float)(((float)mountainStart * 1.25) - mountainStart) *(float)7 + 24) < 31) {
					terrainBMP->setValueAtIndex(pixel, (uint32_t)((float)(altitude - mountainStart) / (float)(210 - mountainStart) *(float)7 + 24));
				}
				else { terrainBMP->setValueAtIndex(pixel, 31); }
			}
		}
		if (displayCounter % updateThreshold == 0 && Data::getInstance().opencvVisualisation) {
			Visualizer::displayImage(terrainBMP->get24BitRepresentation());
		}
	}
	destroyAllWindows();
}
//creates rivers 
void Terrain::generateRivers(Bitmap * riverBMP, Bitmap * heightmap)
{
	cout << "Creating rivers" << endl;
	set<uint32_t> riverPixels;
	//TODO PARAMETRISATION
		//ELEVATION
		//COLOURRANGE
	uint32_t heightmapWidth = heightmap->bInfoHeader.biWidth;
	for (uint32_t i = 0; i < Data::getInstance().numRivers; i++) {
		//start a new river
		River* R = new River();
		this->rivers.push_back(R);

		//find random start point
		uint32_t start = 0;
		while (!(heightmap->getValueAtIndex(start) > Data::getInstance().seaLevel) && riverPixels.find(start) == riverPixels.end())
		{
			start = ((*random)() % heightmap->bInfoHeader.biSizeImage);
		}
		R->setSource(start); //save the source
		R->setcurrentEnd(start); //assign current End, setting it to the start point
		R->pixels.push_back(start);
		riverPixels.insert(R->getCurrentEnd());

		//check each direction for fastest decay in altitude, each direction checked 5 pixels away
		vector<uint32_t> altitudes{ heightmap->getValueAtIndex(ABOVE(R->getCurrentEnd(), heightmapWidth * 5)),
			heightmap->getValueAtIndex(BELOW(R->getCurrentEnd(), heightmapWidth * 5)),
			heightmap->getValueAtIndex(R->getCurrentEnd() - 5),
			heightmap->getValueAtIndex(R->getCurrentEnd() + 5) };

		std::vector<uint32_t>::iterator result = std::min_element(std::begin(altitudes), std::end(altitudes));
		uint32_t favDirection = std::distance(std::begin(altitudes), result);

		int previous = 0; //this variable is used to avoid rectangles in the river
		//continue the river until the sealevel is reached, either at a lake or the ocean
		while (heightmap->getValueAtIndex(R->getCurrentEnd()) > Data::getInstance().seaLevel - 1) {
			uint32_t elevationToleranceOffset = 0;
			vector<uint32_t> candidates;
			//now expand to lower or equal pixel as long as possible
			while (elevationToleranceOffset < Data::getInstance().elevationTolerance && !candidates.size()) {
				if (heightmap->getValueAtIndex(ABOVE(R->getCurrentEnd(), heightmapWidth)) < heightmap->getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(ABOVE(R->getCurrentEnd(), heightmapWidth)) && favDirection != 1) {
					if (ABOVE(R->getCurrentEnd(), heightmapWidth) < riverBMP->bInfoHeader.biSizeImage && previous != -(int)heightmapWidth)
						candidates.push_back(ABOVE(R->getCurrentEnd(), heightmapWidth));
				}
				if (heightmap->getValueAtIndex(BELOW(R->getCurrentEnd(), heightmapWidth)) < heightmap->getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(BELOW(R->getCurrentEnd(), heightmapWidth)) && favDirection != 0) {
					if (BELOW(R->getCurrentEnd(), heightmapWidth) > heightmapWidth && previous != heightmapWidth)
						candidates.push_back(BELOW(R->getCurrentEnd(), heightmapWidth));
				}
				if (heightmap->getValueAtIndex(LEFT(R->getCurrentEnd())) < heightmap->getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(LEFT(R->getCurrentEnd())) && favDirection != 3) {
					if (LEFT(R->getCurrentEnd()) > 3 && previous != 1)
						candidates.push_back(LEFT(R->getCurrentEnd()));
				}
				if (heightmap->getValueAtIndex(RIGHT(R->getCurrentEnd())) < heightmap->getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(RIGHT(R->getCurrentEnd())) && favDirection != 2) {
					if (RIGHT(R->getCurrentEnd()) < heightmap->bInfoHeader.biSizeImage - 1 && previous != -1)
						candidates.push_back(RIGHT(R->getCurrentEnd()));
				}
				if (candidates.size() == 0) {
					elevationToleranceOffset++;
					if (elevationToleranceOffset >= Data::getInstance().elevationTolerance) {
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
			directions.push_back(BELOW(newPixel, heightmapWidth));
			directions.push_back(ABOVE(newPixel, heightmapWidth));
			for (uint32_t index = 0; index < directions.size(); index++)
			{
				if (directions[index] == R->pixels.back())
				{
					directions.erase(directions.begin() + index);
				}
			}
			vector<uint32_t> neighbouringRiverPixels;
			for (auto directionPixel : directions) {
				if (riverPixels.find(directionPixel) != riverPixels.end())
					neighbouringRiverPixels.push_back(directionPixel);
			}


			if (neighbouringRiverPixels.size() > 0) { //newpixel is next to another river, which means we terminate this river inside the other river
				if (R->pixels.size() > 1)
					previous = (R->pixels[R->pixels.size() - 1]) - (R->pixels[R->pixels.size() - 2]);
				R->setcurrentEnd(newPixel);
				R->pixels.push_back(newPixel);
				riverPixels.insert(newPixel);
				for (auto river : rivers)
				{
					if (river->contains(neighbouringRiverPixels[0]))
					{
						river->addIngoing(R, neighbouringRiverPixels[0]);
					}
				}
				//cout << "1::RIVER ENDED IN OTHER river with length: " << R->pixels.size() << endl;
				break;
			}



			bool found = false;
			for (auto river : rivers)
			{
				if (river->contains(newPixel))
				{
					river->addIngoing(R, newPixel);
					found = true;
					break;
				}
			}
			if (!found) {
				//if (riverPixels.find(newPixel) == riverPixels.end()) {
				if (R->pixels.size() > 1)
					previous = (R->pixels[R->pixels.size() - 1]) - (R->pixels[R->pixels.size() - 2]);
				R->setcurrentEnd(newPixel);
				R->pixels.push_back(newPixel);
				riverPixels.insert(newPixel);
			}
			else {
				break;
			}

			if (heightmap->getValueAtIndex(R->getCurrentEnd()) <= Data::getInstance().seaLevel) {
				//cout << "RIVER ENDED IN SEA with length: " << R->pixels.size() << endl;
				break;
			}
		}
	}

	for (uint32_t i = 0; i < rivers.size(); i++)
	{
		if (rivers[i]->pixels.size() < 10) {
			rivers.erase(rivers.begin() + i);
			i--;
		}
	}


	//for (int i = 0; i < provinces.size(); i++)
	//	provinces[i]->computeCandidates();
}

void Terrain::prettyRivers(Bitmap * riverBMP, Bitmap * heightmap)
{
	const uint32_t maxRiverColour = 10;
	for (uint32_t i = 0; i < riverBMP->bInfoHeader.biWidth * riverBMP->bInfoHeader.biHeight; i++)
	{
		if (heightmap->getValueAtIndex(i) > Data::getInstance().seaLevel)
			riverBMP->setValueAtIndex(i, 255);
		else
			riverBMP->setValueAtIndex(i, 254);
	}
	for (River* river : rivers) {
		if (river->pixels.size() < 10)
			continue;
		uint32_t riverColour = 2;
		for (uint32_t pix : river->pixels) {
			if (riverColour < maxRiverColour && river->getIngoingForKey(pix) != nullptr) {
				riverColour += river->getIngoingForKey(pix)->getIngoing().size() + 1;
				if (riverColour > maxRiverColour)
					riverColour = maxRiverColour;
			}
			riverBMP->setValueAtIndex(pix, riverColour);
		}
	}
	for (River* river : rivers) {
		riverBMP->setValueAtIndex(river->getSource(), 0);
	}
}

void Terrain::sanityChecks(Bitmap * provinceBMP)
{
	cout << "Doing terrain sanity checks" << endl;

	for (int i = 0; i < provinceBMP->bInfoHeader.biSizeImage; i++)
	{
		if (provinceBMP->getBuffer()[i] == 0)
			cout << "0 value in provinceBMP " << endl;
	}
	for (uint32_t index = 0; index < provinceBMP->bInfoHeader.biSizeImage / 3; index++) {
		if (provinceBMP->getValueAtIndex(index) == 0 || provinceBMP->getValueAtIndex(index, 1) == 0 || provinceBMP->getValueAtIndex(index, 2) == 0)
			cout << "FATAL ERROR: Unassigned pixel in provinceBMP" << endl;
		if (provinceMap[provinceBMP->getTripleAtIndex(index)] == nullptr)
		{
			cout << "FATAL ERROR: Pixel colourcode results in nullptr" << endl;
		}
	}

	for (auto continent : continents)
	{
		if (continent == nullptr)
		{
			cout << "ERROR: Continent list contains nullptr" << endl;
		}
		else if (continent->provinces.size() == 0)
			cout << "ERROR: Continent " << continent->ID << " has no provinces" << endl;
		if (continent->regions.size() == 0)
			cout << "INFO: Continent " << continent->ID << " has no regions" << endl;
	}
	for (auto region : regions)
	{
		if (region == nullptr)
		{
			cout << "ERROR: region list contains nullptr" << endl;
		}
		if (region->provinces.size() == 0)
		{
			cout << "ERROR: Region " << region->ID << " has no provinces" << endl;
		}
		if (region->country == nullptr)
		{
			cout << "ERROR: Region " << region->ID << " has no country" << endl;
		}
		if (region->continent == nullptr)
		{
			//cout << "Region " << region->ID << " has no continent" << endl;
		}
		for (auto prov : region->provinces)
		{
			if (prov == nullptr)
				cout << "ERROR: Region " << region->ID << " contains nullptr province" << endl;
		}

	}
	for (auto prov : provinces)
	{
		if (prov == nullptr) {
			cout << "ERROR: Province list contains nullptr" << endl;
		}

		if (provinceMap[prov->colour] == nullptr)
		{
			cout << "ERROR: Province colourcode results in nullptr" << endl;
		}
		if (!prov->sea) {
			if (prov->pixels.size() == 0)
				cout << "ERROR: Province has no pixels" << prov->provID << endl;
			if (prov->center == 0)
				cout << "ERROR: Province center is 0" << prov->provID << endl;
			if (prov->continent == nullptr)
				cout << "ERROR: Province has nullptr continent" << prov->provID << endl;
			if (prov->country == nullptr)
				cout << "ERROR: Province has nullptr country: ID: " << prov->provID << endl;
			if (prov->region == nullptr)
				cout << "ERROR: Province has nullptr region" << prov->provID << endl;
		}
		else
		{
			if (prov->country != nullptr)
				cout << "ERROR: Seaprovince has country assigned" << endl;

		}
	}
	if (provinces.back()->provID > provinces.size())
		cout << "ERROR: Higher province IDs than the total amount of provinces. Check deletion of small provinces" << endl;
	if (provinces.size() != Data::getInstance().landProv + Data::getInstance().seaProv)
		cout << "INFO: Amount of provinces diverges from requested amount of provinces by " << (provinces.size() - Data::getInstance().landProv - Data::getInstance().seaProv) << endl;
}
