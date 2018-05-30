#include "Parser.h"



Parser::Parser()
{
}


Parser::~Parser()
{
}

void Parser::writeDefinition(string filePath, vector<Prov*> provinces)
{
	string completeFile = "province;red;green;blue;x;x\n";
	for (int provNr = 0; provNr < provinces.size(); provNr++)
	{
		string line = "";
		line.append(to_string(provNr));
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
		completeFile.append(line);
	}
	ofstream def;
	def.open(filePath);
	def << completeFile;
	def.close();
}

void Parser::writeAdjacency(string filePath, vector<Prov*> provinces)
{
	string completeFile = "provinceID;red;green;blue;Neighbours\n";
	for (int provNr = 0; provNr < provinces.size(); provNr++)
	{
		string line = "";
		line.append(to_string(provinces[provNr]->provnr));
		line.append(";");
		line.append(to_string(provinces[provNr]->colour.rgbtRed));
		line.append(";");
		line.append(to_string(provinces[provNr]->colour.rgbtGreen));
		line.append(";");
		line.append(to_string(provinces[provNr]->colour.rgbtBlue));
		line.append(";");

		for (auto neighbour : provinces[provNr]->neighbourProvinces)
		{
			line.append(to_string(neighbour->provnr));
			line.append(";");
		}
		line.append("\n");
		completeFile.append(line);
	}
	ofstream adj;
	adj.open(filePath);
	adj << completeFile;
	adj.close();
}

void Parser::writeContinents(string filePath, vector<Continent*> continents)
{
	string completeFile = "";
	for (auto continent : continents)
	{
		completeFile.append(continent->name);
		completeFile.append(" = {\n\tprovinces = {\n\t\t");
		for (auto province : continent->provinces)
		{
			completeFile.append(to_string(province->provnr));
			completeFile.append(" ");
		}
		completeFile.append("\n\t}\n\tassimilation_rate = 0.01\n}\n");
	}
	ofstream continent;
	continent.open(filePath);
	continent << completeFile;
	continent.close();
}

void Parser::writeRegions(string filePath, vector<Region*> regions)
{
	string completeFile = "";
	for (auto region : regions)
	{
		completeFile.append(region->name);
		completeFile.append(" = { ");
		for (auto province : region->provinces)
		{
			completeFile.append(to_string(province->provnr));
			completeFile.append(" ");
		}
		completeFile.append("}\n\n");
	}
	ofstream region;
	region.open(filePath);
	region << completeFile;
	region.close();
}

string Parser::readClimateHeader(string filePath)
{
	std::ifstream climate(filePath);
	std::string content((std::istreambuf_iterator<char>(climate)),
		(std::istreambuf_iterator<char>()));
	uint32_t pos = content.find("mild_climate", 100);
	content.erase(pos, string::npos);
	return content;
}

void Parser::writeClimate(string filePath, /*string originalClimatePath,*/ vector<Prov*> provinces)
{
	string completeFile = "";//this is VIC 2 SPECIFIC readClimateHeader(originalClimatePath);
	string mildClimate = "mild_climate = {\n\t";
	string temperateClimate = "temperate_climate = {\n\t";
	string harshClimate = "harsh_climate = {\n\t";
	string inhospitableClimate = "inhospitable_climate = {\n\t";
	for (int provNr = 0; provNr < provinces.size(); provNr++)
	{
		if (provinces[provNr]->climate == "mild_climate")
		{
			mildClimate.append(to_string(provinces[provNr]->provnr));
			mildClimate.append(" ");
		}
		else if (provinces[provNr]->climate == "temperate_climate")
		{
			temperateClimate.append(to_string(provinces[provNr]->provnr));
			temperateClimate.append(" ");
		}
		else if (provinces[provNr]->climate == "harsh_climate")
		{
			harshClimate.append(to_string(provinces[provNr]->provnr));
			harshClimate.append(" ");
		}
		else if (provinces[provNr]->climate == "inhospitable_climate")
		{
			inhospitableClimate.append(to_string(provinces[provNr]->provnr));
			inhospitableClimate.append(" ");
		}
	}
	mildClimate.append("\n\n}\n");
	temperateClimate.append("\n\n}\n");
	harshClimate.append("\n\n}\n");
	inhospitableClimate.append("\n\n}\n");
	completeFile.append(mildClimate);
	completeFile.append(temperateClimate);
	completeFile.append(harshClimate);
	completeFile.append(inhospitableClimate);
	ofstream climate;
	climate.open(filePath);
	climate << completeFile;
	climate.close();
}

string Parser::readDefaultMapHeader(string filePath)
{
	std::ifstream defaultMap(filePath);
	std::string content((std::istreambuf_iterator<char>(defaultMap)),
		(std::istreambuf_iterator<char>()));
	uint32_t pos = content.find("definitions", 100);
	return content.substr(pos, string::npos);
}

void Parser::writeDefaultMapHeader(string filePath, /*string originalDefaultMapPath, */vector<Prov*> provinces)
{
	string completeFile = "max_provinces = " + to_string(provinces.size());
	completeFile.append("\n\nsea_starts = {\n\t");
	for (auto province : provinces) {
		if (province->sea)
		{
			completeFile.append(to_string(province->provnr));
			completeFile.append(" ");
		}
	}
	completeFile.append("\n}\n\n");
	//completeFile.append(readDefaultMapHeader(originalDefaultMapPath));
	ofstream defaultMap;
	defaultMap.open(filePath);
	defaultMap << completeFile;
	defaultMap.close();
}
