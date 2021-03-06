#include "ProvinceGenerator.h"
//MACROS
#define LEFT(val) \
(val-1*1)
#define RIGHT(val) \
(val+1*1)
#define ABOVE(val, offset) \
(val+1*offset)
#define BELOW(val, offset) \
(val-1*offset)

ProvinceGenerator::ProvinceGenerator()
{
	this->random = Data::getInstance().random2;
	randomValuesCached.resize(Data::getInstance().bitmapSize / 16);
	for (auto& val : randomValuesCached) {
		val = Data::getInstance().random2() % 4;
	}
}

ProvinceGenerator::~ProvinceGenerator()
{
}
//creates the province map for fast access of provinces when only
//rgb values are available, removes need to search this province
ProvinceMap ProvinceGenerator::createProvinceMap()
{
	for (auto& province : provinces) {

		provinceMap.setValue(province->colour, province);
	}
	return provinceMap;
}
//
int ProvinceGenerator::GetMinDistanceToProvince(uint32_t position, uint32_t width, uint32_t height) {
	int distance = MAXINT32;
	for (auto& P : provinces)
	{
		const int x1 = P->center % width;
		const int x2 = position % width;
		const int y1 = P->center / height;
		const int y2 = position / height;
		if (sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2))) < distance) {
			distance = (uint32_t)sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
		}
	}
	return distance;
}
//Utility to find starting point of new province
void ProvinceGenerator::determineStartingPixel(Bitmap* bitmap, vector<uint32_t>& provincePixels, RGBTRIPLE& provinceColour, uint32_t provinceSize) {
	const uint32_t bmpWidth = bitmap->bInfoHeader.biWidth;
	const uint32_t bmpHeight = bitmap->bInfoHeader.biHeight;
	const uint32_t bmpSize = bmpWidth * bmpHeight;
	int minDistance = ((double)provinceSize / (double)bmpSize * 500000.0);
	uint32_t startingPixel = random() % bmpSize;//startingpixel is anywhere in the file
	while (bitmap->getValueAtIndex(startingPixel) != provinceColour.rgbtBlue - 1
		|| (GetMinDistanceToProvince(startingPixel, bmpWidth, bmpHeight) < minDistance))
	{
		startingPixel = random() % bmpSize; //startingpixel is anywhere in the file
		minDistance *= 0.7;
	}
	bitmap->setTripleAtIndex(provinceColour, startingPixel);
	provincePixels.push_back(startingPixel);
}
//evaluate if province is coastal
void ProvinceGenerator::evaluateCoasts(Bitmap provinceBMP)
{
	for (auto prov : provinces) {
		if (!prov->sea) {
			for (const auto& neighbour : prov->adjProv)
			{
				if (neighbour->sea)
				{
					prov->coastal = true;
					continue;
				}
			}
		}
	}
}
//Finds neighbours of all provinces and assigns them
void ProvinceGenerator::evaluateNeighbours(Bitmap provinceBMP)
{
	const uint32_t width = provinceBMP.bInfoHeader.biWidth;
	const uint32_t height = provinceBMP.bInfoHeader.biHeight;
	const uint32_t size = width * height;
	for (auto i = 0u; i < size - width; i++)
	{
		Province* left = provinceMap[provinceBMP.getTripleAtIndex(i)];
		Province* right = provinceMap[provinceBMP.getTripleAtIndex(i + 1u)];
		Province* below = provinceMap[provinceBMP.getTripleAtIndex(i + width)];

		if (!(left == right)) {
			left->setNeighbour(right, true);
		}
		else if (!(left == below)) {
			left->setNeighbour(below, true);
		}
	}
}
//Reads Pixels from bitmap and assigns them to provinces
void ProvinceGenerator::provPixels(const Bitmap* provinceBMP)
{
	const uint32_t bmpWidth = provinceBMP->bInfoHeader.biWidth;
	const uint32_t bmpHeight = provinceBMP->bInfoHeader.biHeight;
	const uint32_t bmpSize = bmpWidth * bmpHeight;
	for (auto& prov : provinces)
	{
		prov->pixels.clear();
	}
	vector<unsigned int> newPixels(4);
	for (uint32_t j = 0; j < provinceBMP->bInfoHeader.biSizeImage / 3; j++)
	{
		try {
			provinceMap[provinceBMP->getTripleAtIndex(j)]->pixels.push_back(j);
			newPixels = { RIGHT(j), LEFT(j), ABOVE(j, bmpWidth), BELOW(j, bmpWidth) };
			for (const auto newPixel : newPixels) {
				if (newPixel < bmpSize)
					if (provinceBMP->getValueAtIndex(newPixel, 2) != provinceBMP->getValueAtIndex(j, 2))
					{
						provinceMap[provinceBMP->getTripleAtIndex(j)]->borderPixels.push_back(j);
					}
			}
		}
		catch (runtime_error e)
		{
			const RGBTRIPLE colour = provinceBMP->getTripleAtIndex(j);
			cout << "Accessing province map with r,g,b: " << colour.rgbtRed << " " << colour.rgbtGreen << " " << colour.rgbtBlue << " results in undefined value error" << endl;
		}
	}
}

//generates all land provinces
vector<BYTE> ProvinceGenerator::landProvinces(Bitmap terrainBMP, Bitmap* provinceBMP, Bitmap riverBMP, uint32_t updateThreshold)
{
	cout << "Generating provinces" << endl;
	const uint32_t bmpWidth = terrainBMP.bInfoHeader.biWidth;
	const uint32_t bmpHeight = terrainBMP.bInfoHeader.biHeight;
	const uint32_t bmpSize = bmpWidth * bmpHeight;
	const RGBTRIPLE rgbHigh{ 254, 254, 254 };
	const RGBTRIPLE rgbLow{ 0, 0, 0 };
	//initialize buffer
	for (auto i = 0; i < bmpSize; i++) {
		provinceBMP->setTripleAtIndex(terrainBMP.getValueAtIndex(i) == 254 ? rgbHigh : rgbLow, i);
	}
	//assign province size //better calculation?
	uint32_t landProvSize = (int)((double)bmpSize * 0.6f / (double)Data::getInstance().landProvinceAmount);
	uint32_t seaProvSize = (int)((double)bmpSize * 1.2f / (double)Data::getInstance().seaProvinceAmount);
	//assign pixels to this new province
	//cout << landProvSize << endl;
	provinceCreation(provinceBMP, landProvSize, Data::getInstance().landProvinceAmount, 0, 0);
	provinceCreation(provinceBMP, seaProvSize, Data::getInstance().seaProvinceAmount, Data::getInstance().landProvinceAmount, 254);
	//For multithreading: create vector of random values. Used for performance improvements, as ranlux24 is using locks, and new instances would remove determination.
	uint32_t threadAmount = Data::getInstance().threadAmount;
	//decrement number of threads, until biSizeImage can be divided by threadCount without any rest
	while (bmpSize % threadAmount != 0)
	{
		threadAmount--;
	}
	std::vector<std::thread> threads;
	for (auto i = 0; i < threadAmount; ++i) {
		uint32_t from = i * (bmpSize / threadAmount);
		uint32_t to = (i + 1) * (bmpSize / threadAmount);
		threads.push_back(std::thread(&ProvinceGenerator::fill, this, std::ref(provinceBMP), std::ref(riverBMP), (uint32_t)1, (uint32_t)0, from, to, updateThreshold));
		threads.push_back(std::thread(&ProvinceGenerator::fill, this, std::ref(provinceBMP), std::ref(riverBMP), (uint32_t)255, (uint32_t)254, from, to, updateThreshold));
	}
	//wait for threads to finish
	for (auto& t : threads) {
		t.join();
	}
	assignRemainingPixels(provinceBMP, false);
	assignRemainingPixels(provinceBMP, true);
	threads.clear();
	return provinceBMP->getBuffer();
}
//creates the basic province with a random shape
void ProvinceGenerator::provinceCreation(Bitmap* provinceBMP, uint32_t provinceSize, uint32_t numOfProvs, uint32_t offset, uint32_t greyval)
{
	const uint32_t bmpWidth = provinceBMP->bInfoHeader.biWidth;
	const uint32_t bmpHeight = provinceBMP->bInfoHeader.biHeight;
	const uint32_t bmpSize = bmpWidth * bmpHeight;
	uint32_t red = 0;
	uint32_t green = 0;
	RGBTRIPLE provinceColour;

	for (uint32_t i = offset + 1; i < numOfProvs + offset + 1; i++)
	{
		provinceColour.rgbtRed = 1 + red;
		provinceColour.rgbtGreen = 1 + green;
		provinceColour.rgbtBlue = greyval + 1; //land gets low blue value
		red++;
		if (red > 254) //if end of colourrange(255) is reached
		{
			green++; //increment green value
			red = 1; //reset red
		}
		//create new landprovince
		Province* P = new Province(i, provinceColour, provinceColour.rgbtBlue == 255);

		determineStartingPixel(provinceBMP, P->pixels, provinceColour, provinceSize);
		vector<unsigned int> newPixels(4);
		for (uint32_t x = 0; x < provinceSize - 1; x++)
		{
			//uint32_t currentPixel = P->pixels[P->pixels.size() - random() % ((P->pixels.size() / 4) + 1)];
			uint32_t currentPixel = P->pixels[randomValuesCached[x] % P->pixels.size()];
			newPixels = { RIGHT(currentPixel), LEFT(currentPixel), ABOVE(currentPixel, bmpWidth), BELOW(currentPixel, bmpWidth) };

			for (const auto newPixel : newPixels) {
				if (newPixel < bmpSize && newPixel > 0)
					if (provinceBMP->getValueAtIndex(newPixel) == greyval)
					{
						//if ((newpixel) % (bmpWidth / 2) != 0) {
						provinceBMP->setTripleAtIndex(provinceColour, newPixel);
						P->pixels.push_back(newPixel);
						//lowerSegment++;
						//}
					}
			}
		}
		P->center = P->pixels[0];
		provinces.push_back(P);
	}
}

//fills unassigned pixels in iterations, so provinces grow
void ProvinceGenerator::fill(Bitmap* provinceBMP, const Bitmap riverBMP, const unsigned char greyVal, const unsigned char fillVal, const uint32_t from, const uint32_t to, uint32_t updateThreshold)
{
	cout << "Starting filling of unassigned pixels from pixel " << from << " to pixel " << to << endl;
	const uint32_t bmpWidth = provinceBMP->bInfoHeader.biWidth;
	const uint32_t bmpHeight = provinceBMP->bInfoHeader.biHeight;
	const uint32_t bmpSize = bmpWidth * bmpHeight;
	thread_local uint32_t unassignedPixels = bmpSize;
	uint32_t previousUnassignedPixels = unassignedPixels + 1;
	uint32_t randomValueIndex = 0u;
	const vector<int> offsets = { 1,-1, (int)bmpWidth, -(int)(bmpWidth) };
	int breakCounter = 0;
	int switchCounter = 0;


	vector<unsigned char> blueMap(bmpWidth * bmpHeight);


	while (unassignedPixels > 0)
	{
		//cout << "Pixels still unassigned: " << unassignedPixels << endl;
		previousUnassignedPixels = unassignedPixels;
		unassignedPixels = 0;
		switchCounter++;
		for (int unassignedPixel = from; unassignedPixel < to; unassignedPixel++)
		{
			int accessedPixel = unassignedPixel;

			if (switchCounter % 2 == 1) // we need to change iteration direction, otherwise provinces will look warped
				accessedPixel = (int)to - ((int)unassignedPixel - (int)from) - 1;
			if (provinceBMP->getValueAtIndex(accessedPixel) == fillVal)
				//if (provinceBMP->getBuffer()[accessedPixel*3] == fillVal)
			{
				unassignedPixels++;
				const int direction = randomValuesCached[randomValueIndex++ % randomValuesCached.size()];
				const int newPixel = (int)accessedPixel + offsets[direction];
				if (newPixel < bmpSize && newPixel > 0)
				{
					if (provinceBMP->getValueAtIndex(newPixel) == greyVal)
					{
						//TODO: add river crossings to each province
						if (newPixel % bmpWidth && !(riverBMP.getValueAtIndex(newPixel) <= 10u))
							provinceBMP->copyTripleToIndex(accessedPixel, newPixel);
					}
				}
			}
		}
		unassignedPixels == previousUnassignedPixels ? breakCounter++ : breakCounter = 0;
		//cout << "Previous: " << previousUnassignedPixels << " now: " << unassignedPixels << endl;
		if (breakCounter > 3)
		{
			break;
		}
	}
}
//evaluates province size to define wether it should be deleted in case it is too small
void ProvinceGenerator::beautifyProvinces(Bitmap* provinceBMP, Bitmap riverBMP, uint32_t minProvSize)
{
	cout << "Beautifying provinces" << endl;
	// delete province on map by setting colour to black
	for (const auto& province : provinces)
	{
		if (province->pixels.size() < minProvSize && !province->sea) {
			cout << "Eliminating small province" << endl;
			for (auto pixel : province->pixels) {
				RGBTRIPLE colour{ 0,0,0 };
				provinceBMP->setTripleAtIndex(colour, pixel);
			}
			province->pixels.clear();
			provinceMap.setValue(province->colour, nullptr); // EHHH fix
		}
	}
	// delete provinces in province list
	provinces.erase(std::remove_if(provinces.begin(), provinces.end(), [](const Province* p) {return p->pixels.size() == 0 && !p->sea; }), provinces.end());
	// correct provinceIDs
	for (uint32_t i = 0; i < provinces.size(); i++)
		provinces[i]->provID = i;
	// now assign those small province pixels to nearby provinces
	fill(provinceBMP, std::ref(riverBMP), 1, 0, 0, provinceBMP->bInfoHeader.biSizeImage / 3, 200);
	assignRemainingPixels(provinceBMP, false);
	provPixels(provinceBMP);
}
//assigns all unassigned pixels to the nearest province
void ProvinceGenerator::assignRemainingPixels(Bitmap* provinceBMP, bool sea) {
	cout << "Assigning remaining pixels provinces" << endl;
	//Assign remaining pixels
	const uint32_t bmpWidth = provinceBMP->bInfoHeader.biWidth;
	const uint32_t bmpHeight = provinceBMP->bInfoHeader.biHeight;
	const uint32_t bmpSize = bmpWidth * bmpHeight;

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
					lakeColour.rgbtGreen = 1 + random() % 255;
					lakeColour.rgbtRed = 1 + random() % 255;
					if (provinceMap[lakeColour] == nullptr)
					{
						unique = true;
					}
				}
				Province* lake = new Province((int)provinces.size() + 1, lakeColour, true);
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
						if (i > bmpWidth && provinceBMP->getValueAtIndex(BELOW(i, bmpWidth)) == 254)
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
				auto distance = 10000000;
				Province* nextOwner = nullptr;
				for (Province* P : provinces)
				{
					if ((P->colour.rgbtBlue == 1) /*|| (provinceBuffer[unassignedPixel] == 254 && P->colour.rgbtBlue == 255)*/) {
						//length of vector between current pixel and province pixel
						auto x1 = 0u;
						auto y1 = 0u;
						bool detailed = false;
						if (detailed) {
							auto nearestPixelOfThatProvince = 0u;
							auto pixelDistance = 10000000;
							for (int i = 0; i < (P->pixels.size()); i += 30)
							{
								if (i < P->pixels.size()) {
									const int provincePixel = P->pixels[i];
									const int x1temp = provincePixel % bmpWidth;
									const int x2temp = unassignedPixel % bmpWidth;
									const int y1temp = provincePixel / bmpHeight;
									const int y2temp = unassignedPixel / bmpHeight;
									if (sqrt(((x1temp - x2temp) * (x1temp - x2temp)) + ((y1temp - y2temp) * (y1temp - y2temp))) < pixelDistance) {
										pixelDistance = sqrt(((x1temp - x2temp) * (x1temp - x2temp)) + ((y1temp - y2temp) * (y1temp - y2temp)));
										nearestPixelOfThatProvince = provincePixel;
									}
								}
							}
							x1 = nearestPixelOfThatProvince % bmpWidth;
							y1 = nearestPixelOfThatProvince / bmpHeight;
						}
						else {
							x1 = (P->center) % bmpWidth;
							y1 = (P->center) / bmpHeight;
						}
						auto x2 = (unassignedPixel) % bmpWidth;
						auto y2 = (unassignedPixel) / bmpHeight;
						if (sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2))) < distance) {
							distance = (uint32_t)sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
							nextOwner = P;
						}
					}
				}
				provinceBMP->setTripleAtIndex(nextOwner->colour, unassignedPixel);
			}
		}
	}
}

Province* ProvinceGenerator::getProvinceByID(uint32_t ID)
{
	return provinces[ID];
}

Region& ProvinceGenerator::getRegionByID(uint32_t ID)
{
	return regions[ID];
}

//creates region of defined size on each continent and assigns
//provinces to those regions
void ProvinceGenerator::evaluateRegions(uint32_t minProvPerRegion, uint32_t width, uint32_t height)
{
	uint32_t regionID = 0;
	for (auto prov : provinces)
	{
		if (prov->regionID == 1000000 && !prov->sea) {
			Region R(to_string(regionID), regionID);
			regionID++;
			prov->assignRegion(regionID, true, minProvPerRegion, R);
			regions.push_back(R);
		}
	}
	for (uint32_t i = 0; i < regions.size(); i++)
	{
		if (regions[i].provinces.size() < minProvPerRegion - 1) {
			for (auto province : regions[i].provinces) {
				province->regionID = 1000000;
			}
			regions.erase(regions.begin() + i);
			i--;
		}
	}
	for (int i = 0; i < regions.size(); i++)
	{
		regions[i].ID = i;
		for (auto& prov : regions[i].provinces)
		{
			prov->regionID = i;
		}
	}

	// Grow regions a few times before ignoring adjacencies 
	// (for example overseas provinces that can't find a adjacent province with a region)
	for (auto i = 0u; i < 6; i++)
	{
		for (auto prov : provinces)
		{
			if (prov->regionID == 1000000 && !prov->sea) {
				uint32_t distance = MAXUINT32;
				uint32_t nextOwner = 1000000;
				for (Province* P : provinces)
				{
					if (P->regionID != 1000000) {
						if (getDistance(P->center, prov->center, width, height) < distance)
						{
							if (prov->hasAdjacent(P) || i == 5u)
							{
								distance = getDistance(P->center, prov->center, width, height);
								nextOwner = P->regionID;
							}
						}
					}
				}
				if (nextOwner != 1000000)
				{
					prov->assignRegion(nextOwner, false, minProvPerRegion, getRegionByID(nextOwner));
					getRegionByID(nextOwner).provinces.push_back(prov);
				}
			}
		}
	}
	for (auto& region : regions)
	{
		for (auto& prov : region.provinces)
		{
			for (auto& prov2 : prov->adjProv) {
				if (!prov2->sea && prov2->regionID != regionID)
				{
					region.setNeighbour(prov2->regionID, true);
				}
			}
		}
	}
}
//creates continents from the random landmasses and assigns
//provinces to those continents
void ProvinceGenerator::evaluateContinents(uint32_t minProvPerContinent, uint32_t width, uint32_t height, const TerrainGenerator terrainGen) {
	uint32_t continentID = 0;
	for (const auto& continentPixels : terrainGen.continents)
	{
		Continent C(continentPixels);
		C.ID = continentID++;
		C.name = "";
		continents.push_back(C);
	}

	for (auto& prov : provinces)
	{
		if (!prov->sea)
		{
			for (auto& continent : continents)
			{
				if (continent.findPixel(prov->center)) {
					prov->continentID = continent.ID;
					continent.provinceIDs.push_back(prov->provID);
					regions[prov->regionID].continentID = continent.ID;
					break;
				}
			}
		}
	}
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
}

bool ProvinceGenerator::sanityChecks(const Bitmap& provinceBMP)
{
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
		 if (continent.provinceIDs.size() == 0)
			cout << "ERROR: Continent " << continent.ID << " has no provinces" << endl;
		if (continent.regions.size() == 0)
			cout << "INFO: Continent " << continent.ID << " has no regions" << endl;
	}
	for (auto region : regions)
	{
		if (region.provinces.size() == 0)
		{
			cout << "ERROR: Region " << region.ID << " has no provinces" << endl;
		}
		if (region.country == nullptr)
		{
			cout << "ERROR: Region " << region.ID << " has no country" << endl;
		}
		if (region.continentID == 1000000)
		{
			//cout << "Region " << region->ID << " has no continent" << endl;
		}
		for (auto prov : region.provinces)
		{
			if (prov == nullptr)
				cout << "ERROR: Region " << region.ID << " contains nullptr province" << endl;
		}
	}
	unsigned int coastals(0);
	for (auto prov : provinces)
	{
		if (prov == nullptr) {
			cout << "ERROR: Province list contains nullptr" << endl;
			return false;
		}
		if (provinceMap[prov->colour] == nullptr)
		{
			cout << "ERROR: Province colourcode results in nullptr" << endl;
			return false;
		}
		if (!prov->sea) {
			if (prov->pixels.size() == 0)
			{
				cout << "ERROR: Province has no pixels" << prov->provID << endl;
				return false;
			}
			if (prov->center == 0)
			{
				cout << "ERROR: Province center is 0" << prov->provID << endl;
				return false;
		}
			if (prov->continentID == 1000000)
			{
				//cout << "ERROR: Province has nullptr continent" << prov->provID << endl;
				//return false;
	}
			if (prov->country == nullptr)
			{
				//cout << "ERROR: Province has nullptr country: ID: " << prov->provID << endl;
				//return false;
}
			if (prov->regionID == 1000000)
			{
				//cout << "ERROR: Province has nullptr region" << prov->provID << endl;
				//return false;
			}
		}
		else
		{
			if (prov->country != nullptr)
			{
				cout << "ERROR: Seaprovince has country assigned" << endl;
				return false;
			}
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
	{
		cout << "ERROR: Higher province IDs than the total amount of provinces. Check deletion of small provinces" << endl;
		return false;
	}
	if (provinces.size() != Data::getInstance().landProvinceAmount + Data::getInstance().seaProvinceAmount)
		cout << "INFO: Amount of provinces diverges from requested amount of provinces by " << (int)((int)provinces.size() - (int)Data::getInstance().landProvinceAmount - (int)Data::getInstance().seaProvinceAmount) << endl;

	cout << "INFO: There are a total of " << coastals << " coastal provinces" << endl;
	return true;
}


