#pragma once
#include <fstream>
#include <Windows.h>
#include <iostream>
#include <map>
#include "Bitmap.h"

using namespace std;

class BMPHandler
{
	map<string, Bitmap*> bitmaps;
public:
	BMPHandler();
	~BMPHandler();
	bool SaveBMPToFile(Bitmap*, LPCTSTR bmpfile);
	Bitmap* Load24bitBMP(LPCTSTR input, string key);
	Bitmap* Load8bitBMP(LPCTSTR input, string key);
	Bitmap* findBitmapByKey(string key);

	BMPHandler(BMPHandler const&) = delete;
	void operator=(BMPHandler const&) = delete;

	static BMPHandler& getInstance()
	{
		static BMPHandler instance; // Guaranteed to be destroyed.
		return instance;
	}

};

