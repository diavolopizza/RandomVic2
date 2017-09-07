#pragma once
#include <Windows.h>
#include <vector>
using namespace std;
class Bitmap
{
	
public:
	Bitmap();
	~Bitmap();

	BITMAPINFO bitmapinfo;
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader;

	BYTE* Buffer;
	BYTE*colourtable;
};

