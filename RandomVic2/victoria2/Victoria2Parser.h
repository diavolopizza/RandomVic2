#pragma once
#include <iostream>
#include <fstream>
#include "../entities/Province.h"
class Victoria2Parser
{
public:
	Victoria2Parser();
	~Victoria2Parser();
	static void writeCountries(string modPath, vector<Province*> provinces);
	static void writePops(string modPath, vector<Province*> provinces);

	static void createFolders(string modPath);
	static string readClimateHeader(string modPath);
	static void writeClimate(string modPath, string originalClimatePath, vector<Province*> provinces);
	static string readDefaultMapHeader(string modPath);
	static void writeDefaultMapHeader(string modPath, string originalDefaultMapPath, vector<Province*> provinces);
	static void writePositions(string modPath, vector<Province*> provinces);
	static void writeAdjacencies(string modPath);
};

