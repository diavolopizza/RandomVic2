#pragma once
#include <Windows.h>
#include <vector>
using namespace std;
class BitmapBuffer
{
	uint32_t size;
public:
	BitmapBuffer(uint32_t size);
	~BitmapBuffer();
	BYTE* myBuffer;
	RGBTRIPLE getTriple(uint32_t bufferIndex);
	uint32_t getValueAt(int32_t index, uint32_t mode = 0);
	void setTriple(RGBTRIPLE colour, uint32_t bufferIndex);
	void setTriple(uint32_t bufferIndex, uint32_t bufferIndexNew);
};

