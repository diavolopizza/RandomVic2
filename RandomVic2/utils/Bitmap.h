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

	BITMAPINFO bInfo;
	BITMAPFILEHEADER bFileHeader;
	BITMAPINFOHEADER bInfoHeader;
	Bitmap * get24BitRepresentation();

	unsigned char* colourtable;
	RGBTRIPLE getColourTableEntry(uint32_t index);
	void setBitmapSize(uint32_t width, uint32_t height);

	uint32_t getValueAtIndex(int32_t index, uint32_t mode = 0);
	void setValueAtIndex(uint32_t bufferIndex, uint32_t value);

	RGBTRIPLE getTripleAtIndex(uint32_t bufferIndex);
	void setTripleAtIndex(RGBTRIPLE colour, uint32_t bufferIndex);
	void copyTripleToIndex(uint32_t bufferIndex, uint32_t bufferIndexNew);

	int getValueAtXYPosition(uint32_t heightPos, uint32_t widthPos);
	void setValueAtXYPosition(uint32_t value, uint32_t heightPos, uint32_t widthPos);
	void setTripleAtXYPosition(RGBTRIPLE colour, uint32_t heightPos, uint32_t widthPos);

	BYTE* getBuffer();
	void setBuffer(unsigned char* Buffer);
};

