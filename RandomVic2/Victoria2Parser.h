#pragma once
#include <iostream>
#include <fstream>
#include "entities/Prov.h"
class Victoria2Parser
{
public:
	Victoria2Parser();
	~Victoria2Parser();
	static void writeCountries(string modPath, vector<Prov*> provinces);
	static void writePops(string modPath, vector<Prov*> provinces);

	static string readClimateHeader(string modPath);
	static void writeClimate(string modPath, string originalClimatePath, vector<Prov*> provinces);
	static string readDefaultMapHeader(string modPath);
	static void writeDefaultMapHeader(string modPath, string originalDefaultMapPath, vector<Prov*> provinces);
	static void writePositions(string modPath, vector<Prov*> provinces);
};

