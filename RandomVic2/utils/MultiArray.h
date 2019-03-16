#pragma once
#include "../entities/Prov.h"
#include "boost/multi_array.hpp"
using namespace std;
class MultiArray
{

public:
	typedef boost::multi_array<Prov*, 3> array_type;
	typedef array_type::index index;
	array_type provinceMap;
	void resize(uint32_t dimension);
	MultiArray();
	~MultiArray();
	Prov * operator[] (RGBTRIPLE colour);
	void setValue(RGBTRIPLE colour, Prov * value);
};

