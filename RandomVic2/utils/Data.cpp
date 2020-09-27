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

	//MODULES
	genHeight = root.get<bool>("module.genHeight");
	genSimpleTerrain = root.get<bool>("module.genSimpleTerrain");
	genComplexTerrain = root.get<bool>("module.genComplexTerrain");
	genV2 = root.get<bool>("module.genV2");
	opencvVisualisation = root.get<bool>("module.opencvVisualisation");


	//PATHS
	string victoriaSource = root.get<string>("v2.source");
	VicPath = victoriaSource;
	mapSource = victoriaSource + "map\\";
	modPath = root.get<string>("v2.destination");
	mapDest = modPath + "map\\";

	//MAP PARAMS
	width = root.get<uint32_t>("map.width");
	height = root.get<uint32_t>("map.height");
	seed = root.get<uint32_t>("map.seed");
	seaLevel = root.get<unsigned char>("map.seaLevel");
	landMassPercentage = root.get<uint32_t>("map.landMassPercentage");
	{ //PROVINCE PARAMS
		landProv = root.get<uint32_t>("map.provinces.landProvinces");
		seaProv = root.get<uint32_t>("map.provinces.seaProvinces");
		minProvPerContinent = root.get<uint32_t>("map.provinces.minProvPerContinent");
		minProvPerRegion = root.get<uint32_t>("map.provinces.minProvPerRegion");
		minProvSize = root.get<uint32_t>("map.provinces.minProvSize");
	}

	{ //RIVERS
		numRivers = root.get<uint32_t>("map.rivers.numRivers");
		elevationTolerance = root.get<uint32_t>("map.rivers.elevationTolerance");
	}
	{ //Visualisation
		updateThreshold = root.get<uint32_t>("visualisation.updateThreshold");
	}

	{ //Debug
		debugMapsPath = root.get<string>("debug.debugMapsPath");
	}

	{ //HEIGHTMAP
		//fractalFrequency = root.get<float>("map.heightmap.fractalFrequency");
		//fractalOctaves = root.get<int>("map.heightmap.fractalOctaves");
		//fractalGain = root.get<float>("map.heightmap.fractalGain");
		//divideThreshold = root.get<int>("map.heightmap.divideThreshold");
		complexHeight = root.get<bool>("map.heightmap.complexHeight");
		layerAmount = root.get<uint32_t>("map.heightmap.layerAmount");
		for (uint32_t i = 0; i < layerAmount; i++)
		{
			type.push_back( root.get<uint32_t>("map.heightmap.layers." + to_string(i) + ".type"));
			fractalFrequency.push_back(root.get<double>("map.heightmap.layers." + to_string(i) + ".fractalFrequency"));
			fractalOctaves.push_back(root.get<uint32_t>("map.heightmap.layers." + to_string(i) + ".fractalOctaves"));
			fractalGain.push_back(root.get<double>("map.heightmap.layers." + to_string(i) + ".fractalGain"));
			divideThreshold.push_back(root.get<uint32_t>("map.heightmap.layers." + to_string(i) + ".divideThreshold"));
			weight.push_back(root.get<double>("map.heightmap.layers." + to_string(i) + ".weight"));
			//fractalOctaves = root.get<int>("map.heightmap.fractalOctaves");
			//fractalGain = root.get<float>("map.heightmap.fractalGain");
			//divideThreshold = root.get<int>("map.heightmap.divideThreshold");
		}

	}

	this->random = new ranlux24();
	if (seed)
		random->seed(seed);
	else {
		seed = (uint32_t)time(NULL);
	}
	random->seed(seed);
	cout << "Seeding with " << seed << endl;

}