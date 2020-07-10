#include "CountryGenerator.h"



CountryGenerator::CountryGenerator(Terrain * terrain, ranlux48* random)
{
	this->terrain = terrain;
	this->random = random;
}


CountryGenerator::~CountryGenerator()
{
}



void CountryGenerator::generateCountries(uint32_t amount)
{

	for (int i = 0; i < amount; i++)
	{
		RGBTRIPLE colour = { 120 + (*random)() % 120, 120 + (*random)() % 120, 120 + (*random)() % 120 };
		string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		string tag = "AAA";

		for (int i = 0; i < tag.size(); i++)
		{
			tag[i] = alphabet[(*random)() % 26];
		}
		for (auto c : countriesV)
		{
			if (c->equalColour(colour))
			{
				i--;
				continue;
			}

		}
		if (countriesM.find(tag) != countriesM.end())
		{
			i--;
			continue;
		}

		Country * C = new Country(tag, i, colour, random);
		countriesV.push_back(C);
		C->flag = createFlag();
		string path = "C:\\Users\\Paul\\Documents\\Visual Studio 2017\\Projects\\\RandomVic2\\RandomVic2\\resources\\gfx\\flags\\" + tag + ".tga";
		if (!tga_write_raw(path.c_str(), C->flag->width, C->flag->height, C->flag->flag, TGA_TRUECOLOR_32)) {
			printf("Failed to write image!\n");
			printf(tga_error_string(tga_get_last_error()));
		}
	}

}

Flag * CountryGenerator::createFlag()
{
	Flag * flag = new Flag(random);

	return flag;
}

Country * CountryGenerator::GetClosestCountry(vector<Prov*> provinces, Prov * seekingProv) {
	uint32_t distance = MAXUINT32;
	Country * C = nullptr;
	for (Prov* P : provinces)
	{
		if (!P->country)
			continue;
		uint32_t x1 = P->center % Data::getInstance().width;
		uint32_t x2 = seekingProv->center % Data::getInstance().width;
		uint32_t y1 = P->center / Data::getInstance().height;
		uint32_t y2 = seekingProv->center / Data::getInstance().height;
		if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
			distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
			C = P->country;
		}
	}
	return C;
}

void CountryGenerator::distributeCountries(uint32_t amount, uint32_t sizeVariation, vector<Region*> regions)
{
	generateCountries(amount);
	for (auto C : countriesV)
	{
		uint32_t regionIndex = (*random)() % regions.size();
		if (!regions[regionIndex]->country) {
			C->addRegion(regions[regionIndex]);
			regions[regionIndex]->setCountry(C);
		}
	}
	bool allAssigned = false;
	for (Region * R : regions)
	{
		if (R->country == nullptr)
		{
			for (auto neighbour : R->neighbourRegions)
			{
				if (neighbour->country != nullptr)
				{
					R->setCountry(neighbour->country);
					neighbour->country->addRegion(R);
					break;
				}
			}
		}
	}
	for (Region * R : regions)
	{
		if (R->country == nullptr)
		{
			uint32_t distance = MAXUINT32;
			Country* nextOwner = nullptr;
			for (Region* R2 : regions)
			{
				if (R2->country != nullptr) {
					uint32_t x1 = R2->provinces[0]->center  % Data::getInstance().width;
					uint32_t x2 = R->provinces[0]->center  % Data::getInstance().width;
					uint32_t y1 = R2->provinces[0]->center / Data::getInstance().height;
					uint32_t y2 = R->provinces[0]->center / Data::getInstance().height;
					if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
						distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
						nextOwner = R2->country;
					}
				}
			}
			R->setCountry(nextOwner);
			nextOwner->addRegion(R);
		}
	}
}

Bitmap * CountryGenerator::countryBMP() {

	Bitmap * countryBMP = new Bitmap(Data::getInstance().width, Data::getInstance().height, 24);
	for (auto country : countriesV)
	{
		int countryXspan = country->maxX - country->minX;
		int countryYspan = country->maxY - country->minY;
		for (auto region : country->regions)
			for (auto prov : region->provinces)
			{
				for (auto pixelIndex : prov->pixels)
				{
					//if(pixelIndex % Data::getInstance().width > )
					countryBMP->setTripleAtIndex(country->getColour(), pixelIndex);
					// need to map the position relative to the country dimensions to the position on the flag
					// easy: just determine a  x and y percentage, which can be mapped to the flag
					float xPercent = (float)(pixelIndex % Data::getInstance().width - country->minX) / (float)countryXspan;
					float yPercent = (float)(pixelIndex / Data::getInstance().width - country->minY) / (float)countryYspan;
					int heightpos = country->flag->width * (int)(country->flag->height * yPercent);
					int location = country->flag->width * xPercent + heightpos;
					RGBTRIPLE colour = country->flag->getPixel(location);
					countryBMP->setTripleAtIndex(colour, pixelIndex);

				}
			}
	}
	return countryBMP;
}

Bitmap * CountryGenerator::wealthBMP()
{
	/*need:
		-rivers
		-coastal
		-heat
		-resources
		-industry
		-*/
	return nullptr;
}

Bitmap * CountryGenerator::resourceBMP()
{
	RGBTRIPLE coal = { 0, 0, 0 };
	RGBTRIPLE iron = { 128, 128, 128 };
	RGBTRIPLE sulfur = { 200, 150, 50 };

	RGBTRIPLE fish = { 200, 20, 20 };
	RGBTRIPLE grain = { 255, 255, 0 };
	Bitmap * resourceBMP = new Bitmap(Data::getInstance().width, Data::getInstance().height, 24);
	return resourceBMP;
}

Bitmap * CountryGenerator::civilizationBMP()
{
	Bitmap * civilizationBMP = new Bitmap(Data::getInstance().width, Data::getInstance().height, 24);
	uint32_t conIndex = 0;
	for (auto continent : terrain->continents)
	{
		//lets make half uncivilized and round down.
		if (conIndex < (terrain->continents.size() / 2))
		{
			continent->civilized = true;
		}
		else {
			continent->civilized = false;
		}
		conIndex++;

		//continent->civilized = (bool)(*random)() % 5;
		if (!continent->civilized)
		{
			for (Prov* prov : continent->provinces)
			{
				prov->civilizationLevel = 0.5 + (double)((*random)() % 3) / 10;
			}
		}
		else {
			for (Prov* prov : continent->provinces)
			{
				prov->civilizationLevel = 0.0 + (double)((*random)() % 3) / 10;
			}
		}
	}
	for (int i = 0; i < 3; i++)
		for (auto country : countriesV)
		{
			for (auto region : country->regions)
			{
				for (auto prov : region->provinces)
				{
					prov->civilizationLevel *= (1 + (prov->neighbourProvinces[(*random)() % prov->neighbourProvinces.size()]->civilizationLevel) / 10);
					if (prov->civilizationLevel > 1)
						prov->civilizationLevel = 1;
				}
			}
		}

	for (auto country : countriesV)
	{
		for (auto region : country->regions)
		{
			for (auto prov : region->provinces)
			{
				for (auto pixelIndex : prov->pixels)
				{
					RGBTRIPLE colour = { 255 * prov->civilizationLevel, 255 * prov->civilizationLevel, 0 };
					civilizationBMP->setTripleAtIndex(colour, pixelIndex);
				}
			}
		}
	}
	return civilizationBMP;
}

void CountryGenerator::determineDimensions()
{
	for (auto country : countriesV)
	{
		for (auto province : country->provinces)
		{
			for (auto pixel : province->pixels)
			{
				if (pixel % Data::getInstance().width < country->minX)
				{
					country->minX = pixel % Data::getInstance().width;
				}
				if (pixel % Data::getInstance().width > country->maxX)
				{
					country->maxX = pixel % Data::getInstance().width;
				}

				if (pixel / Data::getInstance().width < country->minY)
				{
					country->minY = pixel / Data::getInstance().width;
				}
				if (pixel / Data::getInstance().width > country->maxY)
				{
					country->maxY = pixel / Data::getInstance().width;
				}
			}
		}
		//cout << country->maxX - country->minX << endl;
		cout << country->maxY - country->minY << endl;
	}
}
