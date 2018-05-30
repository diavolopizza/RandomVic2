#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "../entities/Prov.h"

class Parser
{
public:
	Parser();
	~Parser();


	void writeDefinition(string filePath, vector<Prov*> provinces);
	void writeAdjacency(string filePath, vector<Prov*> provinces);
	void writeContinents(string filePath, vector<Continent*> continents);
	void writeRegions(string filePath, vector<Region*> regions);
	string readClimateHeader(string filePath);
	void writeClimate(string filePath, /*string originalClimatePath,*/ vector<Prov*> provinces);
	string readDefaultMapHeader(string filePath);
	void writeDefaultMapHeader(string filePath, /*string originalDefaultMapPath, */vector<Prov*> provinces);
};

