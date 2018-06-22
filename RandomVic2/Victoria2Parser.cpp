#include "Victoria2Parser.h"



Victoria2Parser::Victoria2Parser()
{
}


Victoria2Parser::~Victoria2Parser()
{
}

void Victoria2Parser::writeCountries(string modPath, vector<Prov*> provinces)
{
	modPath.append("history/provinces/germany/");
	for (auto prov : provinces)
	{
		string completeFile = "";
		ofstream def;
		def.open(modPath + to_string(prov->provID) + ".txt");


		if (prov->sea) {
			def << completeFile;
			def.close();
			continue;
		}

		completeFile.append("owner = " + prov->owner);
		completeFile.append("\n");
		completeFile.append("controller = " + prov->owner);
		completeFile.append("\n");
		completeFile.append("add_core = " + prov->owner);
		completeFile.append("\n");
		completeFile.append("trade_goods = " + prov->tradegood);
		completeFile.append("\n");
		completeFile.append("life_rating = " + to_string(prov->lifeRating));
		completeFile.append("\n");

		def << completeFile;
		def.close();
	}

}

void Victoria2Parser::writePops(string modPath, vector<Prov*> provinces)
{
	vector<string> popTypes = { "aristocrats", "bureaucrats", "officers", "clergymen", "artisans", "soldiers", "farmers" };


	ofstream def;
	string completeFile = "";
	modPath.append("history/pops/1836.1.1/Germany.txt");
	def.open(modPath);
	for (auto prov : provinces)
	{

		if (prov->sea) {
			continue;
		}
		completeFile.append(to_string(prov->provID));
		completeFile.append(" = {\n\t");
		for (auto popType : popTypes)
		{
			completeFile.append(popType + " = { \n\t\t");
			completeFile.append("culture = north_german\n\t\t");
			completeFile.append("religion = protestant\n\t\t");
			completeFile.append("size = 100\n\t}\n\n\t");

		}

		completeFile.append("\n}\n\n");

	}
	def << completeFile;
	def.close();
}
string Victoria2Parser::readClimateHeader(string filePath)
{
	std::ifstream climate(filePath);
	std::string content((std::istreambuf_iterator<char>(climate)),
		(std::istreambuf_iterator<char>()));
	uint32_t pos = content.find("mild_climate", 100);
	content.erase(pos, string::npos);
	return content;
}
void Victoria2Parser::writeClimate(string filePath, string originalClimatePath, vector<Prov*> provinces)
{
	string completeFile = readClimateHeader(originalClimatePath);
	string mildClimate = "mild_climate = {\n\t";
	string temperateClimate = "temperate_climate = {\n\t";
	string harshClimate = "harsh_climate = {\n\t";
	string inhospitableClimate = "inhospitable_climate = {\n\t";
	for (auto province : provinces)
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
	completeFile.append(mildClimate);
	completeFile.append(temperateClimate);
	completeFile.append(harshClimate);
	completeFile.append(inhospitableClimate);
	ofstream climate;
	climate.open(filePath);
	climate << completeFile;
	climate.close();
}

string Victoria2Parser::readDefaultMapHeader(string filePath)
{
	std::ifstream defaultMap(filePath);
	std::string content((std::istreambuf_iterator<char>(defaultMap)),
		(std::istreambuf_iterator<char>()));
	uint32_t pos = content.find("definitions", 100);
	return content.substr(pos, string::npos);
}

void Victoria2Parser::writeDefaultMapHeader(string filePath, string originalDefaultMapPath, vector<Prov*> provinces)
{
	string completeFile = "max_provinces = " + to_string(provinces.size());
	completeFile.append("\n\nsea_starts = {\n\t");
	for (auto province : provinces) {
		if (province->sea)
		{
			completeFile.append(to_string(province->provID));
			completeFile.append(" ");
		}
	}
	completeFile.append("\n}\n\n");
	completeFile.append(readDefaultMapHeader(originalDefaultMapPath));
	ofstream defaultMap;
	defaultMap.open(filePath+"/map/default.map");
	defaultMap << completeFile;
	defaultMap.close();
}

void Victoria2Parser::writePositions(string modPath, vector<Prov*> provinces)
{
	string completeFile = "";
	for (auto province : provinces) {
		completeFile.append(to_string(province->provID));
		completeFile.append(" = \n{\n\t");
		if (!province->sea)
		{
			uint32_t x1temp = province->center / 3 % 5616;
			uint32_t y1temp = province->center / 3 / 5616;
			//UNIT
			completeFile.append("unit=\n\t{\n\t\tx=");
			completeFile.append(to_string(x1temp));
			completeFile.append("\n\t\ty=");
			completeFile.append(to_string(y1temp));
			completeFile.append("\n\t}\n\n\t");
			//TEXT
			completeFile.append("text_position=\n\t{\n\t\tx=");
			completeFile.append(to_string(x1temp));
			completeFile.append("\n\t\ty=");
			completeFile.append(to_string(y1temp));
			completeFile.append("\n\t}\n\ttext_rotation=0\n\ttextscale=1\n\t");
			//building_construction
			completeFile.append("building_construction=\n\t{\n\t\tx=");
			completeFile.append(to_string(x1temp));
			completeFile.append("\n\t\ty=");
			completeFile.append(to_string(y1temp));
			completeFile.append("\n\t}\n\t");
			//military_construction
			completeFile.append("military_construction=\n\t{\n\t\tx=");
			completeFile.append(to_string(x1temp));
			completeFile.append("\n\t\ty=");
			completeFile.append(to_string(y1temp));
			completeFile.append("\n\t}\n\t");
			//military_construction
			completeFile.append("factory=\n\t{\n\t\tx=");
			completeFile.append(to_string(x1temp));
			completeFile.append("\n\t\ty=");
			completeFile.append(to_string(y1temp));
			completeFile.append("\n\t}\n");
			//building_position
			completeFile.append("building_position =\n\t{\n");
			completeFile.append("fort=\n\t{\n\t\tx=");
			completeFile.append(to_string(x1temp));
			completeFile.append("\n\t\ty=");
			completeFile.append(to_string(y1temp));
			completeFile.append("\n\t}\n");
			completeFile.append("naval_base=\n\t{\n\t\tx=");
			completeFile.append(to_string(x1temp));
			completeFile.append("\n\t\ty=");
			completeFile.append(to_string(y1temp));
			completeFile.append("\n\t}\n");
			completeFile.append("railroad=\n\t{\n\t\tx=");
			completeFile.append(to_string(x1temp));
			completeFile.append("\n\t\ty=");
			completeFile.append(to_string(y1temp));
			completeFile.append("\n\t}\n\t}\n}");
			completeFile.append("\n\n");
		}
		else {
			uint32_t x1temp = province->center / 3 % 5616;
			uint32_t y1temp = province->center / 3 / 5616;
			//UNIT
			completeFile.append("unit=\n\t{\n\t\tx=");
			completeFile.append(to_string(x1temp));
			completeFile.append("\n\t\ty=");
			completeFile.append(to_string(y1temp));
			completeFile.append("\n\t}\n\n\t");
			//TEXT
			completeFile.append("text_position=\n\t{\n\t\tx=");
			completeFile.append(to_string(x1temp));
			completeFile.append("\n\t\ty=");
			completeFile.append(to_string(y1temp));
			completeFile.append("\n\t}\n\ttextscale=1\n}\n\n");
		}
	}
	ofstream position;
	position.open(modPath + "map/positions.txt");
	position << completeFile;
	position.close();
}
