#pragma once
#include <vector>
#include <iostream>
#include <unordered_set>
#include <Windows.h>
using namespace std;

class Prov
{

	
	string path;
public:
	Prov();
	Prov(int,RGBTRIPLE);
	Prov(int, RGBTRIPLE,bool);
	~Prov();


	string tag;//tag who owns prov
	int provnr;
	RGBTRIPLE colour;
	int stateid;
	int victory_points = 0;
	bool placed = false, coastal = false, island = false, sea = false;
	bool developed = false;
	int continent = 0;
	bool operator==(const Prov& right) const;
	string terrain_type;
	int center;

	unordered_set <Prov*> neighbours;//contains all the province Ids of the neighbouring provinces
	unordered_set<int> neighbourstates;
	unordered_set<int > pixels;
	class State* state = nullptr;
	void setneighbour2(Prov*P);
	void checkDeveloped(vector <int> developed_continent);
	

	

};

