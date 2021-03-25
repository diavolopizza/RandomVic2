#pragma once
#include <Windows.h>
#include <vector>
#include <iostream>
using namespace std;
class Bitmap
{
private:
	vector<BYTE> Buffer;
	vector<BYTE> colourtable;
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

	RGBTRIPLE getColourTableEntry(const uint32_t index) const;
	void setBitmapSize(const uint32_t width, const uint32_t height);

	unsigned char getValueAtIndex(const uint32_t index, const uint32_t mode = 0) const noexcept;
	void setValueAtIndex(const uint32_t bufIndex, unsigned char value);

	RGBTRIPLE getTripleAtIndex(const uint32_t bufIndex) const;
	void setTripleAtIndex(const RGBTRIPLE colour, uint32_t bufIndex);
	void copyTripleToIndex(uint32_t bufIndex, uint32_t bufIndexNew);

	unsigned char getValueAtXYPosition(const uint32_t heightPos, const  uint32_t widthPos) const;
	void setValueAtXYPosition(const unsigned char value, const uint32_t heightPos, const  uint32_t widthPos);
	void setTripleAtXYPosition(const RGBTRIPLE colour, const uint32_t heightPos, const uint32_t widthPos);

	const vector<BYTE>& getBuffer() const;
	vector<BYTE>& getColourtable();
	const vector<BYTE>& getArea(uint32_t center, uint32_t width, uint32_t height, vector<BYTE> &buff) const;
	void setBuffer(vector<BYTE> Buffer);

	void setIndexFactor(const uint32_t indexFactor);
};

