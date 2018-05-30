#pragma once
using namespace std;
#include "boost/multi_array.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <time.h>
#include <thread>
#include "BMPHandler.h"



class Various1
{

public:
	Various1();
	~Various1();

	//void getrgbval(int, int, int, bool);
	//boost::multi_array<Prov*, 3> provhashes();//reads definitions.csv and calculates the hashes of the rgb values, creates province with provnr,r,g,b,saves it in hashmap
	//void writeadj();//writes the adjacencies into adj.txt
	//void readadj(string source);
	//void provPixels(int filesize);


	//unsigned int numprov = 13205;
	//vector<Prov*> orderedprov;
	//vector <int> seastarts;//contains all province ids of seaprovinces
	//typedef boost::multi_array<Prov*, 3> array_type;
	//typedef array_type::index index;
	//array_type provs;



	////OPTIONS
	//void getoptions();//reads the options.txt
	//void getGeneralOptions();
	//void getIndustryResourceOptions();
	//void getManpowerOptions();
	//void getMilitaryOptions();
	//void getPoliticalOptions();


	//bool generateterrain;
	//unsigned int number_of_countries;
	//bool use_existing_adj;
	//bool provredis;
	//bool redistribute_states;
	//unsigned int number_of_states;
	//bool heightmap;
	//bool countrydis=1;
	//unsigned int numoflandprov;
	//unsigned int numofseaprov;
	//bool simplified;

	//
	//unsigned int global_industry = 0;
	//
	//unsigned int number_of_developed_continents = 0;
	//double industry_mod_undeveloped = 0;
	//double pop_mod_undeveloped = 0;
	//bool random_country_names = 0;
	//bool random_country_flags = 0;

	////manpower
	//unsigned long long global_manpower = 0;
	//unsigned int megalopolis_min_mp=0;
	//unsigned int metropolis_min_mp=0;
	//unsigned int large_city_min_mp=0;
	//unsigned int city_min_mp=0;
	//unsigned int large_town_min_mp=0;
	//unsigned int town_min_mp=0;
	//unsigned int rural_min_mp=0;
	//unsigned int minimum_manpower_in_province = 100000;
	//unsigned int maximum_range_of_mp_in_province = 200000;
	////unsigned int pastoral_min_mp;
	//double coastal_modifier = 0;
	//double urban_modifier=0;
	//double plains_modifier = 0;
	//double desert_modifier = 0;
	//double forest_modifier = 0;
	//double hills_modifier = 0;
	//double mountain_modifier = 0;
	//double marsh_modifier = 0;
	//



	////Resources
	//unsigned int resource_spread = 0;
	//unsigned int steel_availability = 0;
	//unsigned int aluminium_availability = 0;
	//unsigned int oil_availability = 0;
	//unsigned int rubber_availability = 0;
	//unsigned int tungsten_availability = 0;
	//unsigned int chromium_availability = 0;

	//double fascism_resource_modifier = 1;
	//double communism_resource_modifier = 1;
	//double democracy_resource_modifier = 1;
	//double neutral_resource_modifier = 1;

	//double fascism_resource_amount_modifier = 1;
	//double communism_resource_amount_modifier = 1;
	//double democracy_resource_amount_modifier = 1;
	//double neutral_resource_amount_modifier = 1;


	////politics
	//unsigned int fascism_percentage = 0;
	//unsigned int communism_percentage = 0;
	//unsigned int democracy_percentage = 0;
	//unsigned int neutral_percentage = 0;

	////Military
	//double start_division_modifier = 0;
	//double military_fac_percentage = 0;
	//double naval_fac_percentage_in_coastal = 0;
	//boolean no_navy = 0;
	//boolean no_air = 0;

	//double neutral_military_modifier=1;
	//double democracy_military_modifier = 1;
	//double communism_military_modifier=1;
	//double fascism_military_modifier = 1;
};

