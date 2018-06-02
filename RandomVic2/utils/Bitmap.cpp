#include "Bitmap.h"
#include <iostream>


Bitmap::Bitmap()
{
}

Bitmap::Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, BYTE* colourtable)
{
	cout << "BITMAP CONSTRUCTOR" << endl;
	//create bitmap file header
	((unsigned char *)&bitmapfileheader.bfType)[0] = 'B';
	((unsigned char *)&bitmapfileheader.bfType)[1] = 'M';
	bitmapfileheader.bfReserved1 = 0;
	bitmapfileheader.bfReserved2 = 0;

	//create bitmap information header
	bitmapinfoheader.biSize = 40;
	bitmapinfoheader.biPlanes = 1;
	bitmapinfoheader.biCompression = 0;
	bitmapinfoheader.biXPelsPerMeter = 3800;
	bitmapinfoheader.biYPelsPerMeter = 3800;

	if (bitCount == 24) {

		bitmapfileheader.bfOffBits = 54;
		this->bitmapfileheader.bfSize = 54 + height * width * (bitCount / 8);
		this->bitmapinfoheader.biWidth = width;
		this->bitmapinfoheader.biHeight = height;
		this->bitmapinfoheader.biSizeImage = width * height*(bitCount / 8);
		this->Buffer = new BYTE[width*height*(bitCount / 8)];
		this->bitmapinfoheader.biBitCount = 24;
		bitmapinfo.bmiHeader = bitmapinfoheader;
	}
	else {
		bitmapfileheader.bfOffBits = 54 + 256 * 4;
		this->bitmapfileheader.bfSize = 54 + 256 * 4 + height * width * (bitCount / 8);
		this->bitmapinfoheader.biWidth = width;
		this->bitmapinfoheader.biHeight = height;
		this->bitmapinfoheader.biSizeImage = width * height*(bitCount / 8);
		this->Buffer = new BYTE[width*height*(bitCount / 8)];
		this->colourtable = colourtable;
		bitmapinfo.bmiHeader = bitmapinfoheader;
	}
}


Bitmap::~Bitmap()
{
}

RGBTRIPLE Bitmap::getTriple(uint32_t bufferIndex)
{
	if (bitmapinfoheader.biBitCount == 24)
	{
		RGBTRIPLE retVal;
		retVal.rgbtBlue = Buffer[bufferIndex];
		retVal.rgbtGreen = Buffer[bufferIndex + 1];
		retVal.rgbtRed = Buffer[bufferIndex + 2];
		return retVal;
	}
	else {
		return RGBTRIPLE();
	}
}

void Bitmap::setBitmapSize(uint32_t width, uint32_t height)
{
	if (this->bitmapinfoheader.biBitCount == 24) {
		this->bitmapfileheader.bfSize = 54 + height * width * (bitmapinfoheader.biBitCount / 8);
		this->bitmapinfoheader.biWidth = width;
		this->bitmapinfoheader.biHeight = height;
		this->bitmapinfoheader.biSizeImage = width * height*(bitmapinfoheader.biBitCount / 8);
		this->Buffer = new BYTE[width*height*(bitmapinfoheader.biBitCount / 8)];
	}
	else {
		this->bitmapfileheader.bfSize = 54 + 256 * 4 + height * width * (bitmapinfoheader.biBitCount / 8);
		this->bitmapinfoheader.biWidth = width;
		this->bitmapinfoheader.biHeight = height;
		this->bitmapinfoheader.biSizeImage = width * height*(bitmapinfoheader.biBitCount / 8);
		this->Buffer = new BYTE[width*height*(bitmapinfoheader.biBitCount / 8)];
	}
}

uint32_t Bitmap::getValueAt(int32_t index, uint32_t mode)
{
	if (index < 0 || (uint32_t)index > this->bitmapinfoheader.biSizeImage)
		return NULL;
	return Buffer[index + mode];
}

void Bitmap::setTriple(RGBTRIPLE colour, uint32_t bufferIndex)
{
	if (bitmapinfoheader.biBitCount == 24)
	{
		Buffer[bufferIndex] = colour.rgbtBlue;
		Buffer[bufferIndex + 1] = colour.rgbtGreen;
		Buffer[bufferIndex + 2] = colour.rgbtRed;
	}
}

void Bitmap::setTriple(uint32_t bufferIndex, uint32_t bufferIndexNew)
{
	if (bitmapinfoheader.biBitCount == 24)
	{
		Buffer[bufferIndex] = Buffer[bufferIndexNew];
		Buffer[bufferIndex + 1] = Buffer[bufferIndexNew + 1];
		Buffer[bufferIndex + 2] = Buffer[bufferIndexNew + 2];
	}
}

