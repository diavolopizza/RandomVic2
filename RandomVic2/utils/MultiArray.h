#pragma once
#include "../entities/Province.h"
#include "boost/multi_array.hpp"
using namespace std;
class MultiArray
{

public:
	typedef boost::multi_array<Province*, 3> array_type;
	typedef array_type::index index;
	array_type provinceMap;
	void resize(uint32_t dimension);
	MultiArray();
	~MultiArray();
	Province * operator[] (RGBTRIPLE colour);
	void setValue(RGBTRIPLE colour, Province * value);
};

