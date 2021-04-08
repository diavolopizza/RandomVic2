#include "TerrainGenerator.h"

//MACROS
#define LEFT(val) \
(val-1*1)
#define RIGHT(val) \
(val+1*1)
#define ABOVE(val, offset) \
(val+1*offset)
#define BELOW(val, offset) \
(val-1*offset)

TerrainGenerator::TerrainGenerator()
{
	this->random = Data::getInstance().random2;
}

TerrainGenerator::~TerrainGenerator()
{

}

void TerrainGenerator::worleyNoise(vector<BYTE>& layerValues, uint32_t width, uint32_t height)
{
	vector<uint32_t> points;
	for (int i = 0; i < width * height / 10000; i++)
	{
		points.push_back(random() % (width *height));
	}
	int index = 0;
	for (uint32_t x = 0; x < height; x++)
	{
		for (uint32_t y = 0; y < width; y++)
		{
			vector<int> distances = vector<int>(points.size());

			for (int i = 0; i < points.size(); i++)
			{
				distances[i] = getDistance(x*width + y, points[i], width, height);
			}
			sort(distances.begin(), distances.end());

			int scaledDistance = (double)distances[0] / ((double)width * (double)height / 1000000.0);
			scaledDistance = scaledDistance % 255;

			BYTE completeNoise = scaledDistance > 0 ? scaledDistance : 0;
			RGBTRIPLE colour{ completeNoise, completeNoise, completeNoise };
			layerValues[(x*width + y) * 3] = colour.rgbtBlue;
			layerValues[(x*width + y) * 3 + 1] = colour.rgbtGreen;
			layerValues[(x*width + y) * 3 + 2] = colour.rgbtRed;
		}
	}
}
vector<unsigned char> TerrainGenerator::generateLayer(int seed, int layer, double sizeFactor)
{
	auto height = Data::getInstance().height;
	auto width = Data::getInstance().width;
	vector<BYTE> layerValues = vector<BYTE>(width * height * 3 * sizeFactor);
	FastNoiseLite myNoise; // Create a FastNoise object
						   // adjusting frequency is necessary when map size increases, 
						   // as the heightmap will be noisier the larger the map
	const double sizeNoiseFactor = (double)(1024.0 * 1024.0) / (double)(width * height);
	//double sizeNoiseFactor = 1.0 / log2f((double)(width * height)) * 20.0;
	myNoise.SetSeed(Data::getInstance().seed + layer * 100);
	const uint32_t type = Data::getInstance().type[layer];
	cout << sizeNoiseFactor << endl;
	myNoise.SetFrequency((double)Data::getInstance().fractalFrequency[layer] * sizeNoiseFactor);
	myNoise.SetFractalOctaves(Data::getInstance().fractalOctaves[layer]);
	myNoise.SetFractalGain(Data::getInstance().fractalGain[layer]);
	switch (type)
	{
		// regular noisy, frequency around 0.0120 for continent sized shapes
	case 1:
	{
		myNoise.SetNoiseType(FastNoiseLite::NoiseType_Value); // Set the desired noise type
		myNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
		break;
	}
	// regular noisy
	case 2:
	{
		myNoise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic); // Set the desired noise type
		myNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
		break;
	}
	// typical billow, reduce fractal frequency by roughly 85%
	case 3:
	{
		myNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2); // Set the desired noise type
		myNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
		break;
	}
	// typical billow, reduce fractal frequency by roughly 85%
	case 4:
	{
		myNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2); // Set the desired noise type
		myNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
		break;
	}
	}
	unsigned char minHeight; unsigned char maxHeight;

	std::tie(minHeight, maxHeight) = Data::getInstance().heightRange[layer];
	maxHeight /= 2;
	// set the point at which heightvalues are reduced towards 0 
	// this eliminates provinces overlapping at the east/west map boundaries
	const double delimiter = width / Data::getInstance().divideThreshold[layer];
	for (auto x = 0; x < height; x++)
	{
		for (auto y = 0; y < width; y++)
		{
			double xf = (double)x;
			double yf = (double)y;
			double factor = 1;
			//if (yf < delimiter) {
			//	factor = (double)y / (double)delimiter;
			//}
			//else if (yf > width - delimiter)
			//{
			//	factor = ((double)width - (double)yf) / (double)delimiter;
			//}
			double noiseLevel = (double)minHeight + /*largeHeightmap.getValueAtXYPosition(lowerSegment, y) +*/ (myNoise.GetNoise(xf, yf) + 1.0) * (double)maxHeight * factor; // ((-1 to 1) + 1) * 64 * (0 to 1)
			BYTE completeNoise = (BYTE)noiseLevel + static_cast<BYTE>(1u);

			RGBTRIPLE colour{ completeNoise, completeNoise, completeNoise };
			layerValues[(x * width + y) * 3] = colour.rgbtBlue;
			layerValues[(x * width + y) * 3 + 1] = colour.rgbtGreen;
			layerValues[(x * width + y) * 3 + 2] = colour.rgbtRed;
		}
	}
	Bitmap layerBMP(width, height, 24, layerValues);
	BMPHandler::getInstance().SaveBMPToFile(layerBMP, (Data::getInstance().debugMapsPath + ("layers/layer" + to_string(layer) + ".bmp")).c_str());



	return layerValues;
}
//creates the heightmap with a given seed
vector<BYTE> TerrainGenerator::heightMap(uint32_t seed)
{
	double sizeFactor = 1;
	Bitmap largeHeightmap(Data::getInstance().width * sizeFactor, Data::getInstance().height * sizeFactor, 24);
	Bitmap detectedContinents(Data::getInstance().width * sizeFactor, Data::getInstance().height * sizeFactor, 24);
	const auto width = (uint32_t)((double)largeHeightmap.bInfoHeader.biWidth);
	const auto height = (uint32_t)((double)largeHeightmap.bInfoHeader.biHeight);
	cout << "Creating Heightmap" << endl;
	for (auto layer = 0; layer < Data::getInstance().layerAmount; layer++)
	{
		heightmapLayers.push_back(generateLayer(seed, layer, sizeFactor));
	}

	vector<BYTE> worleyLayer = vector<BYTE>(width*height * 3 * sizeFactor);
	//worleyNoise(worleyLayer, width, height);
	//Bitmap worley(width, height, 24, worleyLayer);
	//BMPHandler::getInstance().SaveBMPToFile(worley, (Data::getInstance().debugMapsPath + ("layers/worleyLayer.bmp")).c_str());

	//FOR NORMALIZATION
	largeHeightmap.setBuffer(normalizeHeightMap(largeHeightmap, worleyLayer));

	// NO NORM
	//largeHeightmap.setBuffer((heightmapLayers[0]));

	//for heightmap extraction after collison
	//detectContinents(largeHeightmap);
	//vector<BYTE>buff(Data::getInstance().width * Data::getInstance().height * 3);
	//largeHeightmap.getArea(largeHeightmap.bInfoHeader.biSizeImage / 2, Data::getInstance().width, Data::getInstance().height, buff);


	return largeHeightmap.getBuffer();
}
// normalize the heightmap to values between 0-255
vector<BYTE> TerrainGenerator::normalizeHeightMap(Bitmap heightMap, vector<BYTE> worleyNoise)
{
	double highestValue = 0.0;
	vector<double> combinedValues = vector<double>(heightMap.bInfoHeader.biWidth*heightMap.bInfoHeader.biHeight * 3);
	vector<BYTE> normalisedValues = vector<BYTE>(heightMap.bInfoHeader.biWidth*heightMap.bInfoHeader.biHeight * 3);
	uint32_t index = 0;
	for (const auto& buffer : this->heightmapLayers)
	{
		for (int i = 0; i < heightMap.bInfoHeader.biWidth*heightMap.bInfoHeader.biHeight * 3; i++)
		{
			combinedValues[i] += (double)buffer[i] * Data::getInstance().weight[index];
			if (combinedValues[i] > highestValue)
				highestValue = combinedValues[i];
		}
		index++;
	}
	const double factor = 150.0 / (double)highestValue;
	for (int i = 0; i < heightMap.bInfoHeader.biWidth*heightMap.bInfoHeader.biHeight * 3; i++)
	{
		normalisedValues[i] = (unsigned char)((double)combinedValues[i] * factor);
		//if (normalisedValues[i] > 125)
		//	normalisedValues[i] += worleyNoise[i];
	}
	heightmapLayers.clear();
	return normalisedValues;
}
// Out of basic landmass shape, create continents
void TerrainGenerator::detectContinents(Bitmap terrainBMP)
{
	enum assigned { UNASSIGNED = 1, ASSIGNED = 2 };
	std::cout << "Detecting continents from terrain" << std::endl;
	vector<int> offsets = { 1 ,-1, terrainBMP.bInfoHeader.biWidth, -terrainBMP.bInfoHeader.biWidth };
	int unassignedCounter = 0;
	for (auto i = 0u; i < terrainBMP.bInfoHeader.biSizeImage; i++)
	{
		if (terrainBMP.getValueAtIndex(i) == 13)
		{
			terrainBMP.setValueAtIndex(i, UNASSIGNED);
			unassignedCounter++;
		}
	}
	double landPercentage = (double)unassignedCounter / (double)terrainBMP.bInfoHeader.biSizeImage;

	vector<uint32_t> newContinentPixels(terrainBMP.bInfoHeader.biSizeImage*Data::getInstance().landMassPercentage, 0);
	while (unassignedCounter)
	{
		for (auto i = 0u; i < terrainBMP.bInfoHeader.biSizeImage; i++) //find start
		{
			if (terrainBMP.getValueAtIndex(i) == UNASSIGNED)
			{
				uint32_t sizeCounter = 0;
				newContinentPixels[sizeCounter] = i; //the first pixel of the new continent is at index i
				sizeCounter++;
				terrainBMP.setValueAtIndex(i, ASSIGNED); // this pix is now assigned to a continent
				unassignedCounter--;
				auto pixel = i;
				queue<uint32_t> pixelStack;
				pixelStack.push(pixel);
				while (pixelStack.size())
				{
					pixel = pixelStack.front();
					pixelStack.pop();
					const auto savePixel = pixel;
					for (auto offset : offsets)
					{
						while (terrainBMP.getValueAtIndex(pixel += offset) == UNASSIGNED && (pixel) % terrainBMP.bInfoHeader.biWidth != 0 && (pixel) % terrainBMP.bInfoHeader.biWidth != terrainBMP.bInfoHeader.biWidth - 1)
						{
							if (terrainBMP.getValueAtIndex(pixel) == UNASSIGNED)
							{
								terrainBMP.setValueAtIndex(pixel, ASSIGNED);
								unassignedCounter--;
								pixelStack.push(pixel);
								newContinentPixels[sizeCounter] = pixel;
								sizeCounter++;
							}
							else
								break;
						}
						pixel = savePixel;
					}
				}
				newContinentPixels.resize(sizeCounter);
				std::sort(newContinentPixels.begin(), newContinentPixels.end());
				continents.push_back(newContinentPixels);
				newContinentPixels.resize(terrainBMP.bInfoHeader.biSizeImage*Data::getInstance().landMassPercentage);
				std::fill(newContinentPixels.begin(), newContinentPixels.end(), 0);
			}
		}
	}
	std::sort(continents.begin(), continents.end(), [](const vector<uint32_t>& a, const vector<uint32_t>& b) { return a.size() < b.size(); });

	// cleanup: remove small continents
	for (auto smallContIndex = 0u; smallContIndex < continents.size(); smallContIndex++)
	{
		// percentage of landmass of continent is smaller than 5% of the total landmass
		if ((double)continents[smallContIndex].size() / (double)terrainBMP.bInfoHeader.biSizeImage < landPercentage / 20.0)
		{
			auto distance = MAXINT;
			auto nextCont = 999u;
			// now see which large continent is closer
			for (auto nextContIndex = 0u; nextContIndex < continents.size(); nextContIndex++)
			{
				// skip when same continent, otherwise distance to self is 0
				if (smallContIndex != nextContIndex)
					for (int pix = 0; pix < continents[nextContIndex].size(); pix += 100)
					{
						auto pixDistance = getDistance(continents[nextContIndex][pix], (int)continents[smallContIndex][0], (int)terrainBMP.bInfoHeader.biWidth, (int)terrainBMP.bInfoHeader.biHeight);
						if (pixDistance < distance)
						{
							distance = pixDistance;
							nextCont = nextContIndex;
						}
					}
			}
			continents[nextCont].insert(std::end(continents[nextCont]), std::begin(continents[smallContIndex]), std::end(continents[smallContIndex]));
			// clear too small continent
			continents.erase(continents.begin() + smallContIndex);
			smallContIndex--;
		}
	}

	std::sort(continents.begin(), continents.end(), [](const vector<uint32_t>& a, const vector<uint32_t>& b) { return a.size() < b.size(); });
	for (auto& continent : continents)
	{
		std::sort(continent.begin(), continent.end());
		unsigned char colour = rand() % 255;
		for (const auto& pixel : continent)
		{
			terrainBMP.setValueAtIndex(pixel, colour);
		}
	}
	BMPHandler::getInstance().SaveBMPToFile(terrainBMP, (Data::getInstance().debugMapsPath + ("detectedContinents.bmp")).c_str());
}
//creates the terrain, factoring in heightmap
void TerrainGenerator::createTerrain(Bitmap* terrainBMP, const Bitmap heightMapBmp)
{
	cout << "Creating basic terrain from heightmap" << endl;
	uint32_t corrections = 0;
	double tempLandPercentage = 0;
	while (0.05 < fabs(tempLandPercentage - (double)Data::getInstance().landMassPercentage) && corrections++ < 5) {
		uint32_t landPixels = 0;
		std::cout << tempLandPercentage << endl;
		std::cout << Data::getInstance().landMassPercentage << endl;
		((tempLandPercentage < Data::getInstance().landMassPercentage) ? Data::getInstance().seaLevel -= 3 : Data::getInstance().seaLevel += 3);
		for (auto i = 0u; i < terrainBMP->bInfoHeader.biSizeImage; i++)
		{
			if (heightMapBmp.getValueAtIndex(i) > Data::getInstance().seaLevel) {
				terrainBMP->setValueAtIndex(i, 13);
				landPixels++;
			}
			else {
				terrainBMP->setValueAtIndex(i, 254);
			}
		}

		tempLandPercentage = (double)landPixels / (double)terrainBMP->bInfoHeader.biSizeImage;
		cout << "Sealevel: " << Data::getInstance().seaLevel << endl;
		cout << "Landpercentage: " << tempLandPercentage << endl;
	}
	Data::getInstance().landMassPercentage = tempLandPercentage;
	cout << "Sealevel has been set to " << (unsigned int)Data::getInstance().seaLevel <<
		" to achieve a landMassPercentage of " << Data::getInstance().landMassPercentage << endl;
	for (auto i = 0u; i < terrainBMP->bInfoHeader.biSizeImage; i++)
	{
		if (heightMapBmp.getValueAtIndex(i) > Data::getInstance().seaLevel) {
			terrainBMP->setValueAtIndex(i, 13);
		}
		else {
			terrainBMP->setValueAtIndex(i, 254);
		}
	}
}
//creates rivers 
void TerrainGenerator::generateRivers(const Bitmap heightMap)
{
	cout << "Creating rivers" << endl;
	set<int> riverPixels;
	//TODO PARAMETRISATION
		//ELEVATION
		//COLOURRANGE
	const int heightmapWidth = heightMap.bInfoHeader.biWidth;
	for (int i = 0; i < Data::getInstance().numRivers; i++) {
		//start a new river
		River* R = new River();
		this->rivers.push_back(R);

		//find random start point
		int start = 0;
		while (!(heightMap.getValueAtIndex(start) > Data::getInstance().seaLevel) && riverPixels.find(start) == riverPixels.end())
		{
			start = (random() % heightMap.bInfoHeader.biSizeImage);
		}
		R->setSource(start); //save the source
		R->setcurrentEnd(start); //assign current End, setting it to the start point
		R->pixels.push_back(start);
		riverPixels.insert(R->getCurrentEnd());

		//const int favDirection = 1;

		int previous = 0; //this variable is used to avoid rectangles in the river
		//continue the river until the sealevel is reached, either at a lake or the ocean
		while (heightMap.getValueAtIndex(R->getCurrentEnd()) > Data::getInstance().seaLevel - 1) {
			//check each direction for fastest decay in altitude, each direction checked 5 pixels away
			vector<int> altitudes{ heightMap.getValueAtIndex(ABOVE(R->getCurrentEnd(), heightmapWidth * 5)),
				heightMap.getValueAtIndex(BELOW(R->getCurrentEnd(), heightmapWidth * 5)),
				heightMap.getValueAtIndex(R->getCurrentEnd() - 5),
				heightMap.getValueAtIndex(R->getCurrentEnd() + 5) };

			const std::vector<int>::iterator result = std::min_element(std::begin(altitudes), std::end(altitudes));
			const int favDirection = (int)std::distance(std::begin(altitudes), result);
			int elevationToleranceOffset = 0;
			vector<int> candidates;
			//now expand to lower or equal pixel as long as possible
			while (elevationToleranceOffset < Data::getInstance().elevationTolerance && !candidates.size()) {
				if (heightMap.getValueAtIndex(ABOVE(R->getCurrentEnd(), heightmapWidth)) < heightMap.getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(ABOVE(R->getCurrentEnd(), heightmapWidth))) {
					if (ABOVE(R->getCurrentEnd(), heightmapWidth) < heightMap.bInfoHeader.biSizeImage && previous != -(int)heightmapWidth)
						candidates.push_back(ABOVE(R->getCurrentEnd(), heightmapWidth));
				}
				if (heightMap.getValueAtIndex(BELOW(R->getCurrentEnd(), heightmapWidth)) < heightMap.getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(BELOW(R->getCurrentEnd(), heightmapWidth))) {
					if (BELOW(R->getCurrentEnd(), heightmapWidth) > heightmapWidth && previous != heightmapWidth)
						candidates.push_back(BELOW(R->getCurrentEnd(), heightmapWidth));
				}
				if (heightMap.getValueAtIndex(LEFT(R->getCurrentEnd())) < heightMap.getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(LEFT(R->getCurrentEnd()))) {
					if (LEFT(R->getCurrentEnd()) > 3 && previous != 1)
						candidates.push_back(LEFT(R->getCurrentEnd()));
				}
				if (heightMap.getValueAtIndex(RIGHT(R->getCurrentEnd())) < heightMap.getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(RIGHT(R->getCurrentEnd()))) {
					if (RIGHT(R->getCurrentEnd()) < heightMap.bInfoHeader.biSizeImage - 1 && previous != -1)
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
			int newPixel = candidates[random() % candidates.size()];


			vector<int> directions = { LEFT(newPixel), RIGHT(newPixel), BELOW(newPixel, heightmapWidth),ABOVE(newPixel, heightmapWidth) };
			for (uint32_t index = 0; index < directions.size(); index++)
			{
				if (directions[index] == R->pixels.back())
				{
					directions.erase(directions.begin() + index);
				}
			}
			vector<int> neighbouringRiverPixels;
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

			if (heightMap.getValueAtIndex(R->getCurrentEnd()) <= Data::getInstance().seaLevel) {
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


	//for (int upperSegment = 0; upperSegment < provinces.size(); upperSegment++)
	//	provinces[upperSegment]->computeCandidates();
}

bool TerrainGenerator::sanityChecks()
{
	cout << "Doing terrain sanity checks" << endl;
	for (auto continent : continents)
	{
		if (continent.size() == 0)
		{
			cout << "ERROR: Continent is 0 size, terminating" << endl;
			return false;
		}
	}
	return true;
}
