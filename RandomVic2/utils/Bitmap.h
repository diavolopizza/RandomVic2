#pragma once
#include <Windows.h>
#include <vector>
using namespace std;
class Bitmap
{
private:
	BYTE * Buffer;

public:
	Bitmap();
	Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, BYTE* colourtable = nullptr);
	~Bitmap();

	BITMAPINFO bitmapinfo;
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader;

	BYTE* colourtable;
	RGBTRIPLE getTriple(uint32_t bufferIndex);
	void setBitmapSize(uint32_t width, uint32_t height);
	uint32_t getValueAt(int32_t index, uint32_t mode = 0);
	void setSingle(uint32_t bufferIndex, uint32_t value);
	void setTriple(RGBTRIPLE colour, uint32_t bufferIndex);
	void setTriple(uint32_t bufferIndex, uint32_t bufferIndexNew);
	void setBuffer(BYTE* Buffer);
	BYTE* getBuffer();
};

