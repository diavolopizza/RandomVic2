#include "Buffer.h"



BitmapBuffer::BitmapBuffer(uint32_t size)
{
	this->size = size;
	this->myBuffer = new BYTE[size];
}


BitmapBuffer::~BitmapBuffer()
{
}
RGBTRIPLE BitmapBuffer::getTriple(uint32_t bufferIndex)
{
	RGBTRIPLE retVal;
	retVal.rgbtBlue = myBuffer[bufferIndex];
	retVal.rgbtGreen = myBuffer[bufferIndex + 1];
	retVal.rgbtRed = myBuffer[bufferIndex + 2];
	return retVal;
}
uint32_t BitmapBuffer::getValueAt(int32_t index, uint32_t mode)
{
	if (index < 0 || (uint32_t)index > this->size)
		return NULL;
	return myBuffer[index + mode];
}

void BitmapBuffer::setTriple(RGBTRIPLE colour, uint32_t bufferIndex)
{

	myBuffer[bufferIndex] = colour.rgbtBlue;
	myBuffer[bufferIndex + 1] = colour.rgbtGreen;
	myBuffer[bufferIndex + 2] = colour.rgbtRed;
}

void BitmapBuffer::setTriple(uint32_t bufferIndex, uint32_t bufferIndexNew)
{

	myBuffer[bufferIndex] = myBuffer[bufferIndexNew];
	myBuffer[bufferIndex + 1] = myBuffer[bufferIndexNew + 1];
	myBuffer[bufferIndex + 2] = myBuffer[bufferIndexNew + 2];
}
