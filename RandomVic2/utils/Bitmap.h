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
	Bitmap(uint32_t width, uint32_t height, uint32_t bitCount);
	Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, BYTE * buffer);
	Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, Bitmap bitmap);
	~Bitmap();

	BITMAPINFO bInfo;
	BITMAPFILEHEADER bFileHeader;
	BITMAPINFOHEADER bInfoHeader;
	Bitmap * get24BitRepresentation();

	unsigned char* colourtable;
	RGBTRIPLE getColourTableEntry(uint32_t index) const;
	void setBitmapSize(uint32_t width, uint32_t height);

	unsigned char getValueAtIndex(uint32_t index, const uint32_t mode = 0) const;
	void setValueAtIndex(uint32_t bufferIndex, uint32_t value);

	RGBTRIPLE getTripleAtIndex(uint32_t bufferIndex) const;
	void setTripleAtIndex(RGBTRIPLE colour, uint32_t bufferIndex);
	void copyTripleToIndex(uint32_t bufferIndex, uint32_t bufferIndexNew);

	unsigned char getValueAtXYPosition(uint32_t heightPos, uint32_t widthPos) const;
	void setValueAtXYPosition(uint32_t value, uint32_t heightPos, uint32_t widthPos);
	void setTripleAtXYPosition(RGBTRIPLE colour, uint32_t heightPos, uint32_t widthPos);

	BYTE* getBuffer() const;
	void setBuffer(unsigned char* Buffer);
};

