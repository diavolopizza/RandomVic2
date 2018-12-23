#pragma once
#include <Windows.h>
#include <vector>
#include "boost\multi_array.hpp"
using namespace std;
class Bitmap
{
private:
	BYTE * Buffer;
	Bitmap * bit24Representation = nullptr;


public:
	Bitmap();
	Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, unsigned char* colourtable = nullptr);
	~Bitmap();

	string path;
	BITMAPINFO bitmapinfo;
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader;

	unsigned char* colourtable;
	RGBTRIPLE getColourTableEntry(uint32_t index);
	RGBTRIPLE getTriple(uint32_t bufferIndex);
	void setBitmapSize(uint32_t width, uint32_t height);
	uint32_t getValueAt(int32_t index, uint32_t mode = 0);
	int getValueAtPositions(uint32_t heightPos, uint32_t widthPos);
	void setSingle(uint32_t bufferIndex, uint32_t value);
	void setTriple(RGBTRIPLE colour, uint32_t bufferIndex);
	void setTriple(uint32_t bufferIndex, uint32_t bufferIndexNew);
	void setBuffer(unsigned char* Buffer);
	BYTE* getBuffer();
	Bitmap * get24BitRepresentation();
};

