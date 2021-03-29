#include "ClimateGenerator.h"


ClimateGenerator::ClimateGenerator()
{
	this->random = Data::getInstance().random2;
}

ClimateGenerator::~ClimateGenerator()
{

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
void ClimateGenerator::humidityMap(Bitmap heightmapBMP, Bitmap* humidityBMP, uint32_t seaLevel, uint32_t updateThreshold)
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
void ClimateGenerator::complexTerrain(Bitmap* terrainBMP, const Bitmap heightMap, uint32_t seaLevel, uint32_t updateThreshold)
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
