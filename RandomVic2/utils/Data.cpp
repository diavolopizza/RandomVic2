#include "Data.h"



Data::Data()
{

}


Data::~Data()
{
}


void Data::getConfig(string configPath) {
	// Short alias for this namespace
	namespace pt = boost::property_tree;

	// Create a root
	pt::ptree root;

	pt::read_json(configPath, root);

	//PATHS
	string victoriaSource = root.get<string>("v2.source");
	cout << victoriaSource << endl;
	VicPath = victoriaSource;
	mapSource = victoriaSource + "map\\";
	modPath = root.get<string>("v2.destination");
	mapDest = modPath + "map\\";

	//MAP PARAMS
	width = root.get<int>("map.width");
	height = root.get<int>("map.height");
	seed = root.get<int>("map.seed");
	seaLevel = root.get<int>("map.seaLevel");
	{ //PROVINCE PARAMS
		landProv = root.get<int>("map.provinces.landProvinces");
		seaProv = root.get<int>("map.provinces.seaProvinces");
		minProvPerContinent = root.get<int>("map.provinces.minProvPerContinent");
		minProvPerRegion = root.get<int>("map.provinces.minProvPerRegion");
		minProvSize = root.get<int>("map.provinces.minProvSize");
	}

	{ //RIVERS
		numRivers = root.get<int>("map.rivers.numRivers");
		elevationTolerance = root.get<int>("map.rivers.elevationTolerance");
	}

	{ //HEIGHTMAP
		fractalFrequency = root.get<float>("map.heightmap.fractalFrequency");
		fractalOctaves = root.get<int>("map.heightmap.fractalOctaves");
		fractalGain = root.get<float>("map.heightmap.fractalGain");
	}

	this->random = new ranlux48();
	if (seed)
		random->seed(seed);
	else
		random->seed(time(NULL));

}