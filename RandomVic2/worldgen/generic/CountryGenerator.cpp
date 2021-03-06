#include "CountryGenerator.h"



CountryGenerator::CountryGenerator(ProvinceGenerator *provinceGenerator)
{
	this->provinceGenerator = provinceGenerator;
	this->random = Data::getInstance().random2;
}


CountryGenerator::~CountryGenerator()
{
}



void CountryGenerator::generateCountries(uint32_t amount)
{

	for (uint32_t i = 0; i < amount; i++)
	{
		RGBTRIPLE colour = { 120 + random() % 120, 120 + random() % 120, 120 + random() % 120 };
		string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		string tag = "AAA";

		for (int i = 0; i < tag.size(); i++)
		{
			tag[i] = alphabet[random() % 26];
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
		//C->flag = createFlag();
		//string path = "C:\\Users\\Paul\\Documents\\Visual Studio 2017\\Projects\\\RandomVic2\\RandomVic2\\resources\\gfx\\flags\\" + tag + ".tga";
		//if (!tga_write_raw(path.c_str(), C->flag->width, C->flag->height, C->flag->getFlag(), TGA_TRUECOLOR_32)) {
		//	printf("Failed to write image!\n");
		//	printf(tga_error_string(tga_get_last_error()));
		//}
	}

}

Flag * CountryGenerator::createFlag()
{
	Flag * flag = new Flag(random);

	return flag;
}

Country * CountryGenerator::GetClosestCountry(vector<Province*> provinces, Province * seekingProv) {
	uint32_t distance = MAXUINT32;
	Country * C = nullptr;
	for (Province* P : provinces)
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

void CountryGenerator::distributeCountries(uint32_t amount, uint32_t sizeVariation)
{
	generateCountries(amount);
	for (auto C : countriesV)
	{
		uint32_t regionIndex = random() % provinceGenerator->regions.size();
		if (!provinceGenerator->regions[regionIndex].country) {
			C->addRegion(provinceGenerator->regions[regionIndex].ID);
			provinceGenerator->regions[regionIndex].setCountry(C);
		}
	}
	bool allAssigned = false;
	for (auto R : provinceGenerator->regions)
	{
		if (R.country == nullptr)
		{
			for (auto neighbour : R.neighbourRegions)
			{
				auto& neighbourR = provinceGenerator->getRegionByID(neighbour);
				if (neighbourR.country != nullptr)
				{
					R.setCountry(neighbourR.country);
					neighbourR.country->addRegion(R.ID);
					break;
				}
			}
		}
	}
	for (auto R : provinceGenerator->regions)
	{
		if (R.country == nullptr)
		{
			uint32_t distance = MAXUINT32;
			Country* nextOwner = nullptr;
			for (auto R2 : provinceGenerator->regions)
			{
				if (R2.country != nullptr) {
					uint32_t x1 = R2.provinces[0]->center  % Data::getInstance().width;
					uint32_t x2 = R.provinces[0]->center  % Data::getInstance().width;
					uint32_t y1 = R2.provinces[0]->center / Data::getInstance().height;
					uint32_t y2 = R.provinces[0]->center / Data::getInstance().height;
					if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
						distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
						nextOwner = R2.country;
					}
				}
			}
			R.setCountry(nextOwner);
			nextOwner->addRegion(R.ID);
		}
	}
}

Bitmap CountryGenerator::countryBMP() {

	Bitmap countryBMP(Data::getInstance().width, Data::getInstance().height, 24);
	for (auto country : countriesV)
	{
		int countryXspan = country->maxX - country->minX;
		int countryYspan = country->maxY - country->minY;
		for (auto regionID : country->regionIDs)
		{
			for (auto prov : provinceGenerator->getRegionByID(regionID).provinces)
			{
				for (auto pixelIndex : prov->pixels)
				{
					//if(pixelIndex % Data::getInstance().width > )
					countryBMP.setTripleAtIndex(country->getColour(), pixelIndex);
					// need to map the position relative to the country dimensions to the position on the flag
					// easy: just determine a  x and y percentage, which can be mapped to the flag
					double xPercent = (double)(pixelIndex % Data::getInstance().width - country->minX) / (double)countryXspan;
					double yPercent = (double)(pixelIndex / Data::getInstance().width - country->minY) / (double)countryYspan;
					auto heightpos = (double)country->flag->width * (double)country->flag->height * yPercent;
					auto location = (uint32_t)((double)country->flag->width * xPercent + heightpos);
					RGBTRIPLE colour = country->flag->getPixel(location);
					countryBMP.setTripleAtIndex(colour, pixelIndex);

				}
			}
		}
	}
	return countryBMP;
}

Bitmap CountryGenerator::wealthBMP()
{
	/*need:
		-rivers
		-coastal
		-heat
		-resources
		-industry
		-*/
	return Bitmap();
}

Bitmap CountryGenerator::resourceBMP()
{
	RGBTRIPLE coal = { 0, 0, 0 };
	RGBTRIPLE iron = { 128, 128, 128 };
	RGBTRIPLE sulfur = { 200, 150, 50 };

	RGBTRIPLE fish = { 200, 20, 20 };
	RGBTRIPLE grain = { 255, 255, 0 };
	Bitmap resourceBMP(Data::getInstance().width, Data::getInstance().height, 24);
	return resourceBMP;
}

Bitmap CountryGenerator::civilizationBMP()
{
	Bitmap civilizationBMP(Data::getInstance().width, Data::getInstance().height, 24);
	uint32_t conIndex = 0;
	for (auto& continent : provinceGenerator->continents)
	{
		//lets make half uncivilized and round down.
		if (conIndex < (provinceGenerator->continents.size() / 2))
		{
			continent.civilized = true;
		}
		else {
			continent.civilized = false;
		}
		conIndex++;

		for (auto provID : continent.provinceIDs)
		{
			auto prov = provinceGenerator->getProvinceByID(provID);
			prov->civLevel = (continent.civilized ? 0.0 : 0.5) + (double)(random() % 3) / 10.0;
		}
	}
	for (int i = 0; i < 3; i++)
		for (auto country : countriesV)
		{
			for (auto regionID : country->regionIDs)
			{
				auto& region = provinceGenerator->getRegionByID(regionID);
				for (auto prov : region.provinces)
				{
					if (prov->adjProv.size())
						prov->civLevel *= (1 + (prov->adjProv[random() % prov->adjProv.size()]->civLevel) / 10);
					if (prov->civLevel > 1)
						prov->civLevel = 1;
				}
			}
		}

	for (auto country : countriesV)
	{
		for (auto regionID : country->regionIDs)
		{
			auto& region = provinceGenerator->getRegionByID(regionID);
			for (auto prov : region.provinces)
			{
				for (auto pixelIndex : prov->pixels)
				{
					RGBTRIPLE colour = { static_cast<BYTE>(255.0f * prov->civLevel), static_cast<BYTE>(255.0f * prov->civLevel), static_cast<BYTE>(0) };
					civilizationBMP.setTripleAtIndex(colour, pixelIndex);
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
	}
}

void CountryGenerator::sanityChecks()
{
	for (auto country : countriesV)
	{
		if (country->provinces.size() == 0)
			cout << "ERROR: Country has no provinces" << std::endl;
		if (country->provinces.size() < 5)
			cout << "INFO: Tiny country detected" << std::endl;
		if (country->flag == nullptr)
			cout << "ERROR: Country without flag" << std::endl;

	}

}
