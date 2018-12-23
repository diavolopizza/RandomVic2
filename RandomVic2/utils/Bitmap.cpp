#include "Bitmap.h"
#include <iostream>


Bitmap::Bitmap()
{
}

Bitmap::Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, unsigned char* colourtable)
{
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
		this->Buffer = new unsigned char[width*height*(bitCount / 8)];
		for (int i = 0; i < width*height*(bitCount / 8); i++)
		{
			Buffer[i] = 0;
		}
		this->bitmapinfoheader.biBitCount = 24;
		bitmapinfo.bmiHeader = bitmapinfoheader;
	}
	else {
		bitmapfileheader.bfOffBits = 54 + 256 * 4;
		this->bitmapfileheader.bfSize = 54 + 256 * 4 + height * width * (bitCount / 8);
		this->bitmapinfoheader.biWidth = width;
		this->bitmapinfoheader.biHeight = height;
		this->bitmapinfoheader.biSizeImage = width * height*(bitCount / 8);
		this->Buffer = new unsigned char[width*height*(bitCount / 8)];
		for (int i = 0; i < width*height*(bitCount / 8); i++)
		{
			Buffer[i] = 0;
		}
		this->colourtable = colourtable;
		bitmapinfo.bmiHeader = bitmapinfoheader;
	}
}


Bitmap::~Bitmap()
{
}

RGBTRIPLE Bitmap::getColourTableEntry(uint32_t index)
{
	RGBTRIPLE colour{ colourtable[index*4], colourtable[index * 4 + 1], colourtable[index * 4 + 2] };
	return colour;
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
		this->Buffer = new unsigned char[width*height*(bitmapinfoheader.biBitCount / 8)];
	}
	else {
		this->bitmapfileheader.bfSize = 54 + 256 * 4 + height * width * (bitmapinfoheader.biBitCount / 8);
		this->bitmapinfoheader.biWidth = width;
		this->bitmapinfoheader.biHeight = height;
		this->bitmapinfoheader.biSizeImage = width * height*(bitmapinfoheader.biBitCount / 8);
		this->Buffer = new unsigned char[width*height*(bitmapinfoheader.biBitCount / 8)];
	}
}

uint32_t Bitmap::getValueAt(int32_t index, uint32_t mode)
{
	if (index < 0 || (uint32_t)index > this->bitmapinfoheader.biSizeImage)
		return NULL;
	return Buffer[index + mode];
}

int Bitmap::getValueAtPositions(uint32_t heightPos, uint32_t widthPos)
{
	int position = (heightPos * bitmapinfoheader.biWidth + widthPos) * (bitmapinfoheader.biBitCount / 8);
	if (position < 0 || position > bitmapinfoheader.biSizeImage)
		return -1;
	return Buffer[position];
}

void Bitmap::setSingle(uint32_t bufferIndex, uint32_t value)
{
	this->Buffer[bufferIndex] = value;
}

void Bitmap::setTriple(RGBTRIPLE colour, uint32_t bufferIndex)
{
	if (bitmapinfoheader.biBitCount == 24)
	{
		//if ((int)colour.rgbtBlue > 0)
		//	cout << "here";
		Buffer[bufferIndex] = colour.rgbtBlue;
		colour.rgbtBlue = Buffer[bufferIndex];
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

void Bitmap::setBuffer(unsigned char * Buffer)
{
	this->Buffer = Buffer;
}

unsigned char * Bitmap::getBuffer()
{
	return this->Buffer;
}

Bitmap * Bitmap::get24BitRepresentation()
{
	if (bit24Representation == nullptr)
	{
		bit24Representation = new Bitmap(this->bitmapinfoheader.biWidth, this->bitmapinfoheader.biHeight, 24);
	}
	for (uint32_t i = 0; i < this->bitmapinfoheader.biSizeImage-2000; i++)
	{
		bit24Representation->setSingle(i * 3, this->getColourTableEntry(this->getValueAt(i)).rgbtBlue);
		bit24Representation->setSingle(i * 3 + 1, this->getColourTableEntry(this->getValueAt(i)).rgbtGreen);
		bit24Representation->setSingle(i * 3 + 2, this->getColourTableEntry(this->getValueAt(i)).rgbtRed);
	}
	return bit24Representation;
}

