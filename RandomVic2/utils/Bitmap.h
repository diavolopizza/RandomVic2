#pragma once
#include <Windows.h>
#include <vector>
#include "boost\multi_array.hpp"
using namespace std;
class Bitmap
{
private:
	//BYTE* Buffer = nullptr;
	vector<BYTE> Buffer;
	//std::unique_ptr<BYTE> Buffer;
	//Bitmap bit24Representation;
	uint32_t indexFactor;

public:
	Bitmap();
	Bitmap(uint32_t width, uint32_t height, uint32_t bitCount);
	Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, vector<BYTE> buffer);
	Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, Bitmap bitmap);
	~Bitmap();

	BITMAPINFO bInfo;
	BITMAPFILEHEADER bFileHeader;
	BITMAPINFOHEADER bInfoHeader;
	Bitmap get24BitRepresentation();

	BYTE* colourtable;
	RGBTRIPLE getColourTableEntry(const uint32_t index) const;
	void setBitmapSize(const uint32_t width, const uint32_t height);

	unsigned char getValueAtIndex(const uint32_t index, const uint32_t mode = 0) const;
	void setValueAtIndex(const uint32_t bufIndex, unsigned char value);

	RGBTRIPLE getTripleAtIndex(const uint32_t bufIndex) const;
	void setTripleAtIndex(const RGBTRIPLE colour, uint32_t bufIndex);
	void copyTripleToIndex(uint32_t bufIndex, uint32_t bufIndexNew);

	unsigned char getValueAtXYPosition(const uint32_t heightPos, const  uint32_t widthPos) const;
	void setValueAtXYPosition(const unsigned char value, const uint32_t heightPos, const  uint32_t widthPos);
	void setTripleAtXYPosition(const RGBTRIPLE colour, const uint32_t heightPos, const uint32_t widthPos);

	vector<BYTE>& getBuffer();
	void setBuffer(vector<BYTE> Buffer);

	void setIndexFactor(const uint32_t indexFactor);
};

