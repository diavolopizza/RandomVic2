#include "Parser.h"



Parser::Parser()
{
}


Parser::~Parser()
{
}

void Parser::writeDefinition(string filePath, vector<Prov*> provinces)
{
	string completeString = "province;red;green;blue;x;x\n";
	for (uint32_t provNr = 0; provNr < provinces.size(); provNr++)
	{
		string line = "";
		line.append(to_string(provNr+1));//province IDs start at 1
		line.append(";");
		line.append(to_string(provinces[provNr]->colour.rgbtRed));
		line.append(";");
		line.append(to_string(provinces[provNr]->colour.rgbtGreen));
		line.append(";");
		line.append(to_string(provinces[provNr]->colour.rgbtBlue));
		line.append(";");
		line.append("NAME");
		line.append(";");
		line.append("x");
		line.append("\n");
		completeString.append(line);
	}
	ofstream definitionFile;
	definitionFile.open(filePath);
	definitionFile << completeString;
	definitionFile.close();
}

void Parser::writeAdjacency(string filePath, vector<Prov*> provinces)
{
	string completeString = "provinceID;red;green;blue;Neighbours\n";
	for (uint32_t provNr = 0; provNr < provinces.size(); provNr++)
	{
		string line = "";
		line.append(to_string(provinces[provNr]->provID));
		line.append(";");
		line.append(to_string(provinces[provNr]->colour.rgbtRed));
		line.append(";");
		line.append(to_string(provinces[provNr]->colour.rgbtGreen));
		line.append(";");
		line.append(to_string(provinces[provNr]->colour.rgbtBlue));
		line.append(";");

		for (auto neighbour : provinces[provNr]->adjProv)
		{
			line.append(to_string(neighbour->provID));
			line.append(";");
		}
		line.append("\n");
		completeString.append(line);
	}
	ofstream adjacencyFile;
	adjacencyFile.open(filePath);
	adjacencyFile << completeString;
	adjacencyFile.close();
}

void Parser::writeContinents(string filePath, vector<Continent*> continents)
{
	string completeString = "";
	for (auto continent : continents)
	{
		completeString.append(continent->name);
		completeString.append(" = {\n\tprovinces = {\n\t\t");
		for (auto province : continent->provinces)
		{
			completeString.append(to_string(province->provID));
			completeString.append(" ");
		}
		completeString.append("\n\t}\n\tassimilation_rate = 0.01\n}\n");
	}
	ofstream continentFile;
	continentFile.open(filePath);
	continentFile << completeString;
	continentFile.close();
}

void Parser::writeRegions(string filePath, vector<Region*> regions)
{
	string completeString = "";
	for (auto region : regions)
	{
		completeString.append(region->name);
		completeString.append(" = { ");
		for (auto province : region->provinces)
		{
			completeString.append(to_string(province->provID));
			completeString.append(" ");
		}
		completeString.append("}\n\n");
	}
	ofstream regionFile;
	regionFile.open(filePath);
	regionFile << completeString;
	regionFile.close();
}

string Parser::readClimateHeader(string filePath)
{
	std::ifstream climateFile(filePath);
	std::string content((std::istreambuf_iterator<char>(climateFile)),
		(std::istreambuf_iterator<char>()));
	auto pos = content.find("mild_climate", 100);
	content.erase(pos, string::npos);
	return content;
}

void Parser::writeClimate(string filePath, /*string originalClimatePath,*/ vector<Prov*> provinces)
{
	string completeString = "";//this is VIC 2 SPECIFIC readClimateHeader(originalClimatePath);
	string mildClimate = "mild_climate = {\n\t";
	string temperateClimate = "temperate_climate = {\n\t";
	string harshClimate = "harsh_climate = {\n\t";
	string inhospitableClimate = "inhospitable_climate = {\n\t";
	for (auto province:provinces)
	{
		if (province->sea) {
			continue;
		}
		if (province->climate == "mild_climate")
		{
			mildClimate.append(to_string(province->provID));
			mildClimate.append(" ");
		}
		else if (province->climate == "temperate_climate")
		{
			temperateClimate.append(to_string(province->provID));
			temperateClimate.append(" ");
		}
		else if (province->climate == "harsh_climate")
		{
			harshClimate.append(to_string(province->provID));
			harshClimate.append(" ");
		}
		else if (province->climate == "inhospitable_climate")
		{
			inhospitableClimate.append(to_string(province->provID));
			inhospitableClimate.append(" ");
		}
	}
	mildClimate.append("\n\n}\n");
	temperateClimate.append("\n\n}\n");
	harshClimate.append("\n\n}\n");
	inhospitableClimate.append("\n\n}\n");
	completeString.append(mildClimate);
	completeString.append(temperateClimate);
	completeString.append(harshClimate);
	completeString.append(inhospitableClimate);
	ofstream climateFile;
	climateFile.open(filePath);
	climateFile << completeString;
	climateFile.close();
}

string Parser::readDefaultMapHeader(string filePath)
{
	std::ifstream defaultMapFile(filePath);
	std::string content((std::istreambuf_iterator<char>(defaultMapFile)),
		(std::istreambuf_iterator<char>()));
	auto pos = content.find("definitions", 100);
	return content.substr(pos, string::npos);
}

void Parser::writeDefaultMapHeader(string filePath, /*string originalDefaultMapPath, */vector<Prov*> provinces)
{
	string completeString = "max_provinces = " + to_string(provinces.size());
	completeString.append("\n\nsea_starts = {\n\t");
	for (auto province : provinces) {
		if (province->sea)
		{
			completeString.append(to_string(province->provID));
			completeString.append(" ");
		}
	}
	completeString.append("\n}\n\n");
	//completeFile.append(readDefaultMapHeader(originalDefaultMapPath));
	ofstream defaultMapFile;
	defaultMapFile.open(filePath);
	defaultMapFile << completeString;
	defaultMapFile.close();
}
