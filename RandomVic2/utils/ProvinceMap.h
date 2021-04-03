#pragma once
#include "../entities/Province.h"
#include <map>
using namespace std;
class ProvinceMap
{
	map<uint32_t, Province*> provinceMap;
public:
	constexpr uint32_t to_hash(int r, int g, int b) { return (r << 16) + (g << 8) + b; }
	constexpr RGBTRIPLE to_rgb(int hash) { return { (unsigned char)(hash >> 16), (unsigned char)((hash >> 8) & 0xFF), (unsigned char)(hash & 0xFF) }; }
	ProvinceMap();
	~ProvinceMap();
	Province * operator[] (const RGBTRIPLE colour);
	void setValue(const RGBTRIPLE colour, Province * value);
};

