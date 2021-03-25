#include "Terrain.h"
#include "../utils/BMPHandler.h"
#include "../FastNoiseLite/Cpp/FastNoiseLite.h"
#include <boost/algorithm/clamp.hpp>
#include <thread>


//MACROS
#define LEFT(val) \
(val-1*1)
#define RIGHT(val) \
(val+1*1)
#define ABOVE(val, offset) \
(val+1*offset)
#define BELOW(val, offset) \
(val-1*offset)

Terrain::Terrain()
{
	this->random = Data::getInstance().random2;
}

Terrain::~Terrain()
{

}



//creates the province map for fast access of provinces when only
//rgb values are available, removes need to search this province

//for a given radius, get all pixels in the range around given position 
uint32_t checkRange(uint32_t radius, uint32_t heightPos, uint32_t widthPos, Bitmap RGBBMP, uint32_t width, uint32_t height, uint32_t seaLevel) {
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
	const uint32_t xPos = widthPos;
	const uint32_t yPos = heightPos;
	const uint32_t widthStart = boost::algorithm::clamp(xPos - radius, 0, width);
	const uint32_t widthEnd = boost::algorithm::clamp(xPos + radius, 0, width);
	const uint32_t heightStart = boost::algorithm::clamp(yPos - 10, 0, height);
	const uint32_t heightEnd = boost::algorithm::clamp(yPos + 10, 0, height);

	for (uint32_t heightIndex = heightStart; heightIndex < heightEnd; heightIndex++)
	{
		for (uint32_t widthIndex = widthStart; widthIndex < widthEnd; widthIndex++)
		{
			if (RGBBMP.getValueAtXYPosition(heightIndex, widthIndex) < seaLevel) {
				//inRangePixels.push_back((heightIndex * widthIndex + widthIndex) );
				index++;
			}
		}
	}

	//for (int upperSegment = start; upperSegment < end; upperSegment += 3)
	//{
	//	uint32_t x1 = upperSegment  % width;
	//	uint32_t x2 = position  % width;
	//	uint32_t y1 = upperSegment  / height;
	//	uint32_t y2 = position  / height;
	//	distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
	//	if (distance < radius && largeHeightmap.getValueAt(upperSegment) < 90) {
	//		inRangePixels[index] = upperSegment;
	//		index++;
	//	}
	//}
	return index;
}
//Utility to find starting point of new province

void Terrain::worleyNoise(vector<BYTE>& layerValues, uint32_t width, uint32_t height)
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

vector<BYTE> Terrain::normalizeHeightMap(Bitmap heightMap, vector<BYTE> worleyNoise)
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
		if (normalisedValues[i] > 125)
			normalisedValues[i] += worleyNoise[i];
	}
	heightmapLayers.clear();

	return normalisedValues;
}

void Terrain::detectContinents(Bitmap heightMap)
{
	cout << "Creating basic terrain from heightmap" << endl;
	set<uint32_t> unassignedPixels;
	vector<set<uint32_t>> continents;
	double tempLandPercentage = 0;
	long long value = 0;
	for (auto i = 0u; i < heightMap.bInfoHeader.biSizeImage; i++)
	{
		value += heightMap.getValueAtIndex(i);
	}
	int average = (double)value / (double)(heightMap.bInfoHeader.biSizeImage / 3) *1.1;
	cout << average << endl;
	for (auto i = 0u; i < heightMap.bInfoHeader.biSizeImage / 3; i++)
	{
		if (heightMap.getValueAtIndex(i) > average)
		{
			heightMap.setValueAtIndex(i, 255);
			unassignedPixels.insert(i);
		}
	}
	while (unassignedPixels.size())
	{
		auto pixel = *unassignedPixels.begin();
		set<uint32_t> tempPixels;
		set<uint32_t> pixelStack;
		tempPixels.insert(pixel);
		pixelStack.insert(pixel);
		unassignedPixels.erase(pixel);

		while (pixelStack.size())
		{
			pixel = *pixelStack.begin();
			const auto savePixel = pixel;
			while (heightMap.getValueAtIndex(pixel++) == 255)
			{
				if (tempPixels.find(pixel) == tempPixels.end())
				{
					tempPixels.insert(pixel);
					pixelStack.insert(pixel);
					unassignedPixels.erase(pixel);
				}
				else
					break;
			}
			pixel = savePixel;
			while (heightMap.getValueAtIndex(pixel--) == 255)
			{
				if (tempPixels.find(pixel) == tempPixels.end())
				{
					tempPixels.insert(pixel);
					pixelStack.insert(pixel);
					unassignedPixels.erase(pixel);
				}
				else
					break;
			}
			pixel = savePixel;
			while (heightMap.getValueAtIndex(pixel += heightMap.bInfoHeader.biWidth) == 255)
			{
				if (tempPixels.find(pixel) == tempPixels.end())
				{
					tempPixels.insert(pixel);
					pixelStack.insert(pixel);
					unassignedPixels.erase(pixel);
				}
				else
					break;
			}
			pixel = savePixel;
			while (heightMap.getValueAtIndex(pixel -= heightMap.bInfoHeader.biWidth) == 255)
			{
				if (tempPixels.find(pixel) == tempPixels.end())
				{
					tempPixels.insert(pixel);
					pixelStack.insert(pixel);
					unassignedPixels.erase(pixel);
				}
				else
					break;
			}
			pixelStack.erase(savePixel);
		}

		unsigned char blue = rand() % 255;
		for (const auto& pixel : tempPixels)
		{
			heightMap.setValueAtIndex(pixel, blue);
		}
		continents.push_back(tempPixels);
	}
	BMPHandler::getInstance().SaveBMPToFile(heightMap, (Data::getInstance().debugMapsPath + ("detectedContinents.bmp")).c_str());
}

//creates the heightmap with a given seed
vector<BYTE> Terrain::heightMap(uint32_t seed)
{
	double sizeFactor = 1;
	Bitmap largeHeightmap(Data::getInstance().width * sizeFactor, Data::getInstance().height * sizeFactor, 24);
	Bitmap detectedContinents(Data::getInstance().width * sizeFactor, Data::getInstance().height * sizeFactor, 24);
	const auto width = (uint32_t)((double)largeHeightmap.bInfoHeader.biWidth);
	const auto height = (uint32_t)((double)largeHeightmap.bInfoHeader.biHeight);
	cout << "Creating Heightmap" << endl;
	for (auto layer = 0; layer < Data::getInstance().layerAmount; layer++)
	{
		vector<BYTE> layerValues = vector<BYTE>(width*height * 3 * sizeFactor);
		FastNoiseLite myNoise; // Create a FastNoise object
		// adjusting frequency is necessary when map size increases, 
		// as the heightmap will be noisier the larger the map
		const double sizeNoiseFactor = (double)(1024.0 * 1024.0) / (double)(width * height);
		//double sizeNoiseFactor = 1.0 / log2f((double)(width * height)) * 20.0;
		myNoise.SetSeed(seed + layer * 100);
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
				layerValues[(x*width + y) * 3] = colour.rgbtBlue;
				layerValues[(x*width + y) * 3 + 1] = colour.rgbtGreen;
				layerValues[(x*width + y) * 3 + 2] = colour.rgbtRed;
			}
		}
		heightmapLayers.push_back(layerValues);
		Bitmap layerBMP(width, height, 24, layerValues);
		BMPHandler::getInstance().SaveBMPToFile(layerBMP, (Data::getInstance().debugMapsPath + ("layers/layer" + to_string(layer) + ".bmp")).c_str());
	}

	/*vector<BYTE> layerValues = vector<BYTE>(width*height * 3 * sizeFactor);
	worleyNoise(layerValues, width, height);
	Bitmap worley(width, height, 24, layerValues);
	BMPHandler::getInstance().SaveBMPToFile(worley, (Data::getInstance().debugMapsPath + ("layers/worleyLayer.bmp")).c_str());*/

	//FOR NORMALIZATION
	//largeHeightmap.setBuffer(normalizeHeightMap(largeHeightmap, heightmapLayers));

	// NO NORM
	largeHeightmap.setBuffer((heightmapLayers[0]));

	//for heightmap extraction after collison
	//detectContinents(largeHeightmap);
	//vector<BYTE>buff(Data::getInstance().width * Data::getInstance().height * 3);
	//largeHeightmap.getArea(largeHeightmap.bInfoHeader.biSizeImage / 2, Data::getInstance().width, Data::getInstance().height, buff);


	return largeHeightmap.getBuffer();
}
//creates the terrain, factoring in heightmap
void Terrain::createTerrain(Bitmap* terrainBMP, const Bitmap heightMapBmp)
{
	cout << "Creating basic terrain from heightmap" << endl;
	uint32_t corrections = 0;
	double tempLandPercentage = 0;
	while (0.05 < fabs(tempLandPercentage - (double)Data::getInstance().landMassPercentage / 100.0) && corrections++ < 2) {
		uint32_t landPixels = 0;
		((tempLandPercentage > Data::getInstance().landMassPercentage) ? Data::getInstance().seaLevel -= 3 : Data::getInstance().seaLevel += 3);
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
		cout << "Landpercentage: " << tempLandPercentage << endl;
	}
	cout << "Sealevel has been set to " << (unsigned int)Data::getInstance().seaLevel <<
		" to achieve a landMassPercentage of " << tempLandPercentage << endl;
}


double calcMountainShadowAridity(Bitmap heightmapBMP, uint32_t heightPos, uint32_t widthPos, int currentDirection, uint32_t seaLevel, double windIntensity) {
	const uint32_t width = heightmapBMP.bInfoHeader.biWidth;
	//in regions with low windintensity, this effect has a lower range
	const uint32_t maxEffectDistance = (uint32_t)(((double)width / 100.0) * windIntensity);
	uint32_t mountainPixelsInRange = 0;
	for (uint32_t i = 0; i < maxEffectDistance; i++)
	{
		//TODO
		//if (heightmapBMP.getValueAtIndex((heightPos * width + widthPos + (currentDirection * upperSegment)) ) > seaLevel * 1.4)
		//{
		//	mountainPixelsInRange++;
		//}
	}
	return windIntensity * ((double)mountainPixelsInRange / (double)maxEffectDistance);
}
double calcCoastalHumidity(Bitmap heightmapBMP, uint32_t heightPos, uint32_t widthPos, int windDirection, uint32_t seaLevel, double windIntensity, uint32_t width, uint32_t height) {
	//the more continental, the less of an influence the distance to a coast has
	uint32_t continentality = 0;
	//East/west directions are more important that north/south, as important winds travel east/west more often
	const uint32_t maxEffectDistance = width / 200;
	//in the opposite direction of the major global winds, the distance to look at is much larger
	const auto windFactor = 20.0 * windIntensity;
	uint32_t coastDistance = (uint32_t)((double)maxEffectDistance * windFactor);
	double windDistance = maxEffectDistance * windFactor;

	//the direction opposite to the winds, e.g. west in case of west winds(winds coming from the west)
	for (uint32_t i = 0; i < maxEffectDistance * windFactor; i++)
	{
		unsigned char value = heightmapBMP.getValueAtXYPosition(heightPos, widthPos + (i * windDirection));
		if (value != -1 && value < seaLevel)
		{
			if (i < windDistance)
				windDistance = i;
		}
	}
	for (uint32_t x = 0; x < maxEffectDistance * windFactor; x++) {
		unsigned char value = heightmapBMP.getValueAtXYPosition(heightPos, widthPos + (uint32_t)((windDistance + (double)x) * (double)windDirection));
		if (value != -1 && value > seaLevel) {
			continentality++;
		}
	}
	//the direction opposite to the winds, e.g. west in case of west winds(winds coming from the west)
	for (uint32_t i = 0; i < maxEffectDistance; i++)
	{
		unsigned char value = heightmapBMP.getValueAtXYPosition(heightPos, widthPos + (i * windDirection));
		if (value != -1 && value < seaLevel)
		{
			if (i < coastDistance)
				coastDistance = i;
		}

	}
	//the direction the winds are going(e.g. east in case of west winds)
	for (uint32_t i = 0; i < maxEffectDistance; i++)
	{
		unsigned char value = heightmapBMP.getValueAtXYPosition(heightPos, widthPos + (i * windDirection * -1));
		if (value != -1 && value < seaLevel)
		{
			if (i < coastDistance)
				coastDistance = i;
		}
	}
	//north
	for (uint32_t i = 0; i < maxEffectDistance; i++)
	{
		unsigned char value = heightmapBMP.getValueAtXYPosition(heightPos + i, widthPos);
		if (value != -1 && value < seaLevel)
		{
			if (i < coastDistance)
				coastDistance = i;
		}
	}
	//south
	for (uint32_t i = 0; i < maxEffectDistance; i++)
	{
		unsigned char value = heightmapBMP.getValueAtXYPosition(heightPos - i, widthPos);
		if (value != -1 && value < seaLevel)
		{
			if (i < coastDistance)
				coastDistance = i;
		}
	}
	//the more is returned, the more humid the pixel
	//find the smallest factor:
		//higher means less humid
	const double continentalityFactor = 1 * (((double)continentality / ((double)maxEffectDistance* windFactor)));
	//higher means less humid
	const double windDistanceFactor = (((double)windDistance / ((double)maxEffectDistance * windFactor)));// *windIntensity;
	//higher means less humid
	const double coastDistanceFactor = (double)coastDistance / (double)maxEffectDistance;
	if (windDistanceFactor + continentalityFactor > coastDistanceFactor)
		return 1 - coastDistanceFactor;
	else return 1 - (windDistanceFactor + continentalityFactor);

}
void Terrain::humidityMap(Bitmap heightmapBMP, Bitmap* humidityBMP, uint32_t seaLevel, uint32_t updateThreshold)
{
	//if (Data::getInstance().opencvVisualisation)
	//	Visualizer::initializeWindow();
	cout << "Creating humidity map" << endl;
	constexpr double polarEasterlies = 0.3;
	constexpr double westerlies = 0.7;
	constexpr double tradeWinds = 1;
	const uint32_t width = heightmapBMP.bInfoHeader.biWidth;
	const uint32_t height = heightmapBMP.bInfoHeader.biHeight;

	for (uint32_t x = 0; x < height; x++)
	{
		int windDirection = 1;
		//the height of the image scaled between 0 and 2
		const double heightf = (double)x / ((double)height / 2);

		double windIntensity = 0;
		//if the heightf is ...
		if (heightf == boost::algorithm::clamp(heightf, polarEasterlies, westerlies) || heightf == boost::algorithm::clamp(heightf, (double)(2 - westerlies), (double)(2 - polarEasterlies)))
		{   // ... in westerlies range
			windDirection = -1;
			if (heightf == boost::algorithm::clamp(heightf, polarEasterlies, westerlies))
				windIntensity = 0.4 + (0.6 * (1 - ((heightf - polarEasterlies) / (westerlies - polarEasterlies))));
			else if (heightf == boost::algorithm::clamp(heightf, (double)(2 - westerlies), (double)(2 - polarEasterlies))) {
				const double a = (heightf - (1.99 - westerlies));
				const double b = (2 - polarEasterlies) - (1.99 - westerlies);
				windIntensity = 0.4 + (0.6 * ((a / b)));
			}
		}
		else if (heightf == boost::algorithm::clamp(heightf, westerlies, tradeWinds) || heightf == boost::algorithm::clamp(heightf, (double)(2 - tradeWinds), (double)(2 - westerlies)))
		{   //... in tradewinds range
			windDirection = 1;
			if (heightf == boost::algorithm::clamp(heightf, westerlies, tradeWinds))
				windIntensity = 0.4 + (0.6 * (((heightf - westerlies) / (tradeWinds - westerlies))));
			else if (heightf == boost::algorithm::clamp(heightf, (double)(2 - tradeWinds), (double)(2 - westerlies))) {
				const double a = (heightf - (1.99 - tradeWinds));
				const double b = (2 - westerlies) - (1.99 - tradeWinds);
				windIntensity = 0.4 + (0.6 * (1 - (a / b)));
			}
		}
		else if (heightf == boost::algorithm::clamp(heightf, 0, polarEasterlies) || heightf == boost::algorithm::clamp(heightf, (double)(2 - polarEasterlies), (double)(2)))
		{   // ... in polar easterlies range
			windDirection = 1;
			if (heightf == boost::algorithm::clamp(heightf, 0, polarEasterlies))
				windIntensity = 0.4 + (0.6 * (((heightf) / (polarEasterlies))));
			else if (heightf == boost::algorithm::clamp(heightf, (double)(2 - polarEasterlies), (double)(2))) {
				const double a = (heightf - (1.99 - polarEasterlies));
				const double b = (2) - (1.99 - polarEasterlies);
				windIntensity = 0.4 + (0.6 * (1 - (a / b)));
			}
		}
		for (uint32_t y = 0; y < width; y++)
		{
			//TODO
			if (heightmapBMP.getValueAtIndex((x * width + y)) > seaLevel)
			{
				//the higher, the drier
				const double mountainShadowAridity = calcMountainShadowAridity(heightmapBMP, x, y, windDirection, seaLevel, windIntensity);
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
				totalAridity = totalAridity * 0.2 + 0.8 * (totalAridity * (1.0 - coastalHumidity));
				//cannot get drier than super dry or less dry than humid
				totalAridity = boost::algorithm::clamp(totalAridity, 0, 1);

				{
					RGBTRIPLE colour = { static_cast<BYTE>(255.0 * (1.0 - totalAridity)),static_cast<BYTE>(totalAridity * 255.0),static_cast<BYTE>(totalAridity * 255.0) };
					humidityBMP->setTripleAtIndex(colour, (x * width + y));
				}
			}
		}
	}

	/*uint32_t smoothDistance = 2;
	for (int upperSegment = width  * smoothDistance + smoothDistance; upperSegment < humidityBMP.bInfoHeader.biSizeImage - width  * smoothDistance - smoothDistance; upperSegment += 3)
	{
		RGBTRIPLE north = humidityBMP.getTripleAtIndex(upperSegment - width  * smoothDistance);
		RGBTRIPLE northwest = humidityBMP.getTripleAtIndex(upperSegment - width  * smoothDistance - smoothDistance );
		RGBTRIPLE northeast = humidityBMP.getTripleAtIndex(upperSegment - width  * smoothDistance + smoothDistance );
		RGBTRIPLE south = humidityBMP.getTripleAtIndex(upperSegment + width  * smoothDistance);
		RGBTRIPLE southwest = humidityBMP.getTripleAtIndex(upperSegment + width  * smoothDistance - smoothDistance );
		RGBTRIPLE southeast = humidityBMP.getTripleAtIndex(upperSegment + width  * smoothDistance + smoothDistance );
		RGBTRIPLE west = humidityBMP.getTripleAtIndex(upperSegment + smoothDistance );
		RGBTRIPLE east = humidityBMP.getTripleAtIndex(upperSegment - smoothDistance );
		RGBTRIPLE colour;
		colour.rgbtBlue = (double)((int)north.rgbtBlue + (int)northwest.rgbtBlue + (int)northeast.rgbtBlue + (int)southwest.rgbtBlue + (int)southeast.rgbtBlue + (int)south.rgbtBlue + (int)west.rgbtBlue + (int)east.rgbtBlue) / 8;
		colour.rgbtGreen = (double)((int)north.rgbtGreen + (int)northwest.rgbtGreen + (int)northeast.rgbtGreen + (int)southwest.rgbtGreen + (int)southeast.rgbtGreen + (int)south.rgbtGreen + (int)west.rgbtGreen + (int)east.rgbtGreen) / 8;
		colour.rgbtRed = (double)((int)north.rgbtRed + (int)northwest.rgbtRed + (int)northeast.rgbtRed + (int)southwest.rgbtRed + (int)southeast.rgbtRed + (int)south.rgbtRed + (int)west.rgbtRed + (int)east.rgbtRed) / 8;

		humidityBMP.setTripleAtIndex(colour, upperSegment);
	}*/
}
//creates terrain around simplistic climate model
void Terrain::prettyTerrain(Bitmap* terrainBMP, const Bitmap heightMap, uint32_t seaLevel, uint32_t updateThreshold)
{
	/*cout << "Creating complex terrain" << endl;
	if (Data::getInstance().opencvVisualisation)
		Visualizer::initializeWindow();
	//TODO CONFIG FOR ALL PARAMS
	const uint32_t coastalDistanceInfluence = 30;
	const unsigned char mountainStart = seaLevel + (uint32_t)((double)seaLevel * 0.4);
	const unsigned char hillStart = seaLevel + (uint32_t)((double)seaLevel * 0.2);

	constexpr unsigned char arctic = 0;//0-3
	constexpr unsigned char arcticRange = 4;
	constexpr unsigned char plains = 5;
	constexpr unsigned char plainsRange = 4;
	constexpr unsigned char farmlands = 8;
	constexpr unsigned char farmlandsRange = 4;
	constexpr unsigned char forest = 12;
	constexpr unsigned char forestRange = 4;
	constexpr unsigned char hills = 16;
	constexpr unsigned char woods = 20;
	constexpr unsigned char mountains = 24; //24-31
	constexpr unsigned char plains2 = 32;
	constexpr unsigned char steppe = 36;
	constexpr unsigned char steppeRange = 4;
	constexpr unsigned char jungle = 40;
	constexpr unsigned char jungleRange = 4;
	constexpr unsigned char marsh = 44;
	constexpr unsigned char marshRange = 4;
	constexpr unsigned char desert = 48;
	constexpr unsigned char desertRange = 4;
	constexpr unsigned char coastal_desert = 52;
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
		const uint32_t equator = heightMap.bInfoHeader.biHeight / 2;
		const uint32_t pixX = prov->center % (heightMap.bInfoHeader.biWidth);
		const uint32_t pixY = (prov->center - pixX) / heightMap.bInfoHeader.biWidth;
		double temperatureFactor = (double)(pixY % equator) / (double)equator;


		//DISTANCE TO COAST
		double humidityFactor = 0;
		int offset = 0;
		int distanceNorth = 0;
		int distanceSouth = 0;
		int distanceEast = 0;
		int distanceWest = 0;
		const uint32_t bitmapWidth = terrainBMP->bInfoHeader.biWidth;
		//TODO
		while (terrainBMP->getValueAtIndex(prov->center + offset) != 254) {
			offset += 3;
			distanceEast = offset;
		}
		offset = 0;
		while (terrainBMP->getValueAtIndex(prov->center - offset) != 254) {
			offset += 3;
			distanceWest = offset;
		}
		offset = bitmapWidth;
		while (terrainBMP->getValueAtIndex(prov->center + offset) != 254) {
			offset += 3 * bitmapWidth;
			distanceNorth = (uint32_t)((double)offset / (double)bitmapWidth);
		}
		offset = bitmapWidth;
		while (terrainBMP->getValueAtIndex(prov->center - offset) != 254) {
			offset += 3 * bitmapWidth;
			distanceSouth = (uint32_t)((double)offset / (double)bitmapWidth);
		}
		uint32_t distances[] = { abs(distanceNorth),abs(distanceEast),
								abs(distanceWest), abs(distanceSouth) };

		if (*std::min_element(distances, distances + 4) < coastalDistanceInfluence) {
			humidityFactor = (double)(1 - (*std::min_element(distances, distances + 4) / coastalDistanceInfluence));
		}
		//DISTANCE TO EQUATOR
		if (pixY >= equator) {
			temperatureFactor = 1.0 - temperatureFactor;
		}
		if (temperatureFactor < 0.1f && !prov->sea)//mountains
		{
			arcticLikelyhood += 1.0;
		}
		else if (temperatureFactor < 0.3f && temperatureFactor >= 0.1f && !prov->sea)
		{
			arcticLikelyhood += 0.3f - temperatureFactor;//0.2-0
			forestLikelyhood += 0.1f + temperatureFactor;//0.2-0.4
		}
		else if (temperatureFactor < 0.6f && temperatureFactor >= 0.3f && !prov->sea)
		{
			farmlandsLikelyhood += temperatureFactor - 0.3;//0.0-0.3
			marshLikelyhood += 0.03f;
			forestLikelyhood += (0.7f - temperatureFactor);//0.4-0.1
		}
		else if (temperatureFactor < 0.75f && temperatureFactor >= 0.6f && !prov->sea)
		{
			farmlandsLikelyhood += 0.9f - temperatureFactor;//0.3-0.15
			steppeLikelyhood += temperatureFactor - 0.6f;//0.0-0.15
		}
		else if (temperatureFactor < 0.85f && temperatureFactor >= 0.75f && !prov->sea)
		{
			steppeLikelyhood += humidityFactor * (temperatureFactor - 0.6);//0.15-0.25
			farmlandsLikelyhood += 0.5f*humidityFactor;
			desertLikelyhood += 0.5f*(1.0 - humidityFactor);//
		}
		else if (temperatureFactor >= 0.85f && !prov->sea)
		{
			steppeLikelyhood += 1.0 - temperatureFactor;//0.25-0.1
			jungleLikelyhood += (temperatureFactor - 0.85f) * 2.0;//0.0-0.7
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
		for (auto likelyhood : likelyHoods)
		{
			sumOfAllLikelyhoods += likelyhood;
		}

		vector<bool> candidates;
		for (auto likelyHood : likelyHoods) {
			candidates.push_back(false);
		}

		for (auto candidate : candidates) {
			candidate = false;
		}
		auto foundAtLeastOne = false;
		while (!foundAtLeastOne) {
			for (uint32_t i = 0; i < likelyHoods.size(); i++) {
				if (random() % 100u < (likelyHoods[i] / sumOfAllLikelyhoods) * 100.0)
				{
					candidates[i] = true;
					foundAtLeastOne = true;
				}
			}
		}
		auto pick = false;
		auto index = 0u;
		while (!pick)
		{
			index = random() % candidates.size();
			pick = candidates[index];
		}
		for (auto pixel : prov->pixels)
		{
			auto altitude = heightMap.getValueAtIndex(pixel);
			switch (index) {
			case 0:
			{
				terrainBMP->setValueAtIndex(pixel, arctic + random() % arcticRange);
				prov->climate = "inhospitable_climate";
				break;
			}
			case 1:
			{
				terrainBMP->setValueAtIndex(pixel, plains + random() % plainsRange);
				prov->climate = "temperate_climate";
				break;
			}
			case 2:
			{
				terrainBMP->setValueAtIndex(pixel, forest + (unsigned char)((double)forestRange*(1.0 - (temperatureFactor / 0.6))));
				prov->climate = "temperate_climate";
				if (temperatureFactor < 0.3f)
					prov->climate = "harsh_climate";
				break;
			}
			case 3:
			{
				terrainBMP->setValueAtIndex(pixel, farmlands + random() % farmlandsRange);
				prov->climate = "mild_climate";
				break;
			}
			case 4:
			{
				terrainBMP->setValueAtIndex(pixel, steppe + random() % steppeRange);
				prov->climate = "harsh_climate";
				break;
			}
			case 5:
			{
				terrainBMP->setValueAtIndex(pixel, jungle + random() % jungleRange);
				prov->climate = "harsh_climate";
				break;
			}
			case 6:
			{
				terrainBMP->setValueAtIndex(pixel, marsh + random() % marshRange);
				prov->climate = "temperate_climate";
				break;
			}
			case 7:
			{
				terrainBMP->setValueAtIndex(pixel, desert + random() % desertRange);
				prov->climate = "inhospitable_climate";
				break;
			}
			}
			if (altitude > hillStart)//mountains
			{
				terrainBMP->setValueAtIndex(pixel, (uint32_t)((double)(altitude - hillStart) / (double)(mountainStart - hillStart) * 4.0 + 16.0));
			}
			if (altitude > mountainStart)//mountains
			{
				if ((uint32_t)((double)(altitude - mountainStart) / (double)(((double)mountainStart * 1.25f) - mountainStart) *(double)7.0 + 24.0) < 31u) {
					terrainBMP->setValueAtIndex(pixel, (uint32_t)((double)(altitude - mountainStart) / (double)(210u - mountainStart) *7.0 + 24u));
				}
				else { terrainBMP->setValueAtIndex(pixel, 31u); }
			}
		}
	}*/
}
//creates rivers 
void Terrain::generateRivers(Bitmap* riverBMP, const Bitmap heightMap)
{
	cout << "Creating rivers" << endl;
	set<uint32_t> riverPixels;
	//TODO PARAMETRISATION
		//ELEVATION
		//COLOURRANGE
	const uint32_t heightmapWidth = heightMap.bInfoHeader.biWidth;
	for (uint32_t i = 0; i < Data::getInstance().numRivers; i++) {
		//start a new river
		River* R = new River();
		this->rivers.push_back(R);

		//find random start point
		uint32_t start = 0;
		while (!(heightMap.getValueAtIndex(start) > Data::getInstance().seaLevel) && riverPixels.find(start) == riverPixels.end())
		{
			start = (random() % heightMap.bInfoHeader.biSizeImage);
		}
		R->setSource(start); //save the source
		R->setcurrentEnd(start); //assign current End, setting it to the start point
		R->pixels.push_back(start);
		riverPixels.insert(R->getCurrentEnd());

		//check each direction for fastest decay in altitude, each direction checked 5 pixels away
		vector<uint32_t> altitudes{ heightMap.getValueAtIndex(ABOVE(R->getCurrentEnd(), heightmapWidth * 5)),
			heightMap.getValueAtIndex(BELOW(R->getCurrentEnd(), heightmapWidth * 5)),
			heightMap.getValueAtIndex(R->getCurrentEnd() - 5),
			heightMap.getValueAtIndex(R->getCurrentEnd() + 5) };

		const std::vector<uint32_t>::iterator result = std::min_element(std::begin(altitudes), std::end(altitudes));
		const uint32_t favDirection = (uint32_t)std::distance(std::begin(altitudes), result);

		int previous = 0; //this variable is used to avoid rectangles in the river
		//continue the river until the sealevel is reached, either at a lake or the ocean
		while (heightMap.getValueAtIndex(R->getCurrentEnd()) > Data::getInstance().seaLevel - 1) {
			uint32_t elevationToleranceOffset = 0;
			vector<uint32_t> candidates;
			//now expand to lower or equal pixel as long as possible
			while (elevationToleranceOffset < Data::getInstance().elevationTolerance && !candidates.size()) {
				if (heightMap.getValueAtIndex(ABOVE(R->getCurrentEnd(), heightmapWidth)) < heightMap.getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(ABOVE(R->getCurrentEnd(), heightmapWidth)) && favDirection != 1) {
					if (ABOVE(R->getCurrentEnd(), heightmapWidth) < riverBMP->bInfoHeader.biSizeImage && previous != -(int)heightmapWidth)
						candidates.push_back(ABOVE(R->getCurrentEnd(), heightmapWidth));
				}
				if (heightMap.getValueAtIndex(BELOW(R->getCurrentEnd(), heightmapWidth)) < heightMap.getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(BELOW(R->getCurrentEnd(), heightmapWidth)) && favDirection != 0) {
					if (BELOW(R->getCurrentEnd(), heightmapWidth) > heightmapWidth && previous != heightmapWidth)
						candidates.push_back(BELOW(R->getCurrentEnd(), heightmapWidth));
				}
				if (heightMap.getValueAtIndex(LEFT(R->getCurrentEnd())) < heightMap.getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(LEFT(R->getCurrentEnd())) && favDirection != 3) {
					if (LEFT(R->getCurrentEnd()) > 3 && previous != 1)
						candidates.push_back(LEFT(R->getCurrentEnd()));
				}
				if (heightMap.getValueAtIndex(RIGHT(R->getCurrentEnd())) < heightMap.getValueAtIndex(R->getCurrentEnd()) + elevationToleranceOffset && !R->contains(RIGHT(R->getCurrentEnd())) && favDirection != 2) {
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
			uint32_t newPixel = candidates[random() % candidates.size()];


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



void Terrain::sanityChecks(Bitmap provinceBMP)
{
	/*cout << "Doing terrain sanity checks" << endl;

	for (uint32_t i = 0; i < provinceBMP.bInfoHeader.biSizeImage; i++)
	{
		if (provinceBMP.getBuffer()[i] == 0)
			cout << "0 value in provinceBMP " << endl;
	}
	for (uint32_t index = 0; index < provinceBMP.bInfoHeader.biSizeImage / 3; index++) {
		if (provinceBMP.getValueAtIndex(index) == 0 || provinceBMP.getValueAtIndex(index, 1) == 0 || provinceBMP.getValueAtIndex(index, 2) == 0)
			cout << "FATAL ERROR: Unassigned pixel in provinceBMP" << endl;
		if (provinceMap[provinceBMP.getTripleAtIndex(index)] == nullptr)
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
	unsigned int coastals(0);
	for (auto prov : provinces)
	{
		if (prov == nullptr) {
			cout << "ERROR: Province list contains nullptr" << endl;
			continue;
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
		if (prov->pixels.size() < Data::getInstance().minProvSize)
		{

			cout << (prov->sea ? "WARNING: " : "ERROR: ") << "Small province remaining, Colour: " << (int)prov->colour.rgbtRed << " " << (int)prov->colour.rgbtGreen << " " << (int)prov->colour.rgbtBlue << endl;
			cout << "\tCenter at: " << prov->center << " size is: " << prov->pixels.size() << std::endl;
		}
		//if (!prov->adjProv.size())
		//	cout << "Province with ID: " << prov->provID << " has 0 adjacent provinces" << endl;
		if (prov->coastal)
			coastals++;
	}
	if (provinces.back()->provID > provinces.size())
		cout << "ERROR: Higher province IDs than the total amount of provinces. Check deletion of small provinces" << endl;
	if (provinces.size() != Data::getInstance().landProv + Data::getInstance().seaProv)
		cout << "INFO: Amount of provinces diverges from requested amount of provinces by " << (int)((int)provinces.size() - (int)Data::getInstance().landProv - (int)Data::getInstance().seaProv) << endl;

	cout << "INFO: There are a total of " << coastals << " coastal provinces" << endl;*/
}
