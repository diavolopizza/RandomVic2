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
	bool SaveBMP(Bitmap*, LPCTSTR bmpfile, int height, int width, string type);
	bool SaveChangedBMP(Bitmap*, LPCTSTR bmpfile);
	Bitmap* Load24bitBMP(LPCTSTR input, int modifier, string key);
	Bitmap* Load8bitBMP(LPCTSTR input, int modifier, string key);
	Bitmap* findBitmapByKey(string key);



	BMPHandler(BMPHandler const&) = delete;
	void operator=(BMPHandler const&) = delete;

	static BMPHandler& getInstance()
	{
		static BMPHandler   instance; // Guaranteed to be destroyed.
							  // Instantiated on first use.
		return instance;
	}

};

