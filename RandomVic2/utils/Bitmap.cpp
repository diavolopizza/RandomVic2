#include "Bitmap.h"
#include <iostream>


Bitmap::Bitmap()
{
}

Bitmap::Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, unsigned char* colourtable)
{
	//create bitmap file header
	((unsigned char *)&bFileHeader.bfType)[0] = 'B';
	((unsigned char *)&bFileHeader.bfType)[1] = 'M';
	bFileHeader.bfReserved1 = 0;
	bFileHeader.bfReserved2 = 0;

	//create bitmap information header
	bInfoHeader.biSize = 40;
	bInfoHeader.biPlanes = 1;
	bInfoHeader.biCompression = 0;
	bInfoHeader.biXPelsPerMeter = 3800;
	bInfoHeader.biYPelsPerMeter = 3800;

	if (bitCount == 24) {

		bFileHeader.bfOffBits = 54;
		this->bFileHeader.bfSize = 54 + height * width * (bitCount / 8);
		this->bInfoHeader.biWidth = width;
		this->bInfoHeader.biHeight = height;
		this->bInfoHeader.biSizeImage = width * height*(bitCount / 8);
		this->Buffer = new unsigned char[width*height*(bitCount / 8)];
		for (int i = 0; i < width*height*(bitCount / 8); i++)
		{
			Buffer[i] = 0;
		}
		this->bInfoHeader.biBitCount = 24;
		bInfo.bmiHeader = bInfoHeader;
	}
	else {
		bFileHeader.bfOffBits = 54 + 256 * 4;
		this->bFileHeader.bfSize = 54 + 256 * 4 + height * width * (bitCount / 8);
		this->bInfoHeader.biWidth = width;
		this->bInfoHeader.biHeight = height;
		this->bInfoHeader.biSizeImage = width * height*(bitCount / 8);
		this->Buffer = new unsigned char[width*height*(bitCount / 8)];
		for (int i = 0; i < width*height*(bitCount / 8); i++)
		{
			Buffer[i] = 0;
		}
		this->colourtable = colourtable;
		bInfo.bmiHeader = bInfoHeader;
	}
}


Bitmap::~Bitmap()
{
}

RGBTRIPLE Bitmap::getColourTableEntry(uint32_t index)
{
	RGBTRIPLE colour{ colourtable[index * 4], colourtable[index * 4 + 1], colourtable[index * 4 + 2] };
	return colour;
}

RGBTRIPLE Bitmap::getTripleAtIndex(uint32_t bufferIndex)
{
	if (bInfoHeader.biBitCount == 24)
	{
		bufferIndex *= 3;
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
	if (this->bInfoHeader.biBitCount == 24) {
		this->bFileHeader.bfSize = 54 + height * width * (bInfoHeader.biBitCount / 8);
		this->bInfoHeader.biWidth = width;
		this->bInfoHeader.biHeight = height;
		this->bInfoHeader.biSizeImage = width * height*(bInfoHeader.biBitCount / 8);
		this->Buffer = new unsigned char[width*height*(bInfoHeader.biBitCount / 8)];
	}
	else {
		this->bFileHeader.bfSize = 54 + 256 * 4 + height * width * (bInfoHeader.biBitCount / 8);
		this->bInfoHeader.biWidth = width;
		this->bInfoHeader.biHeight = height;
		this->bInfoHeader.biSizeImage = width * height*(bInfoHeader.biBitCount / 8);
		this->Buffer = new unsigned char[width*height*(bInfoHeader.biBitCount / 8)];
	}
}

uint32_t Bitmap::getValueAtIndex(int32_t index, uint32_t mode)
{
	if (bInfoHeader.biBitCount == 24)
	{
		index *= 3;
	}
	if (index < 0 || (uint32_t)index > this->bInfoHeader.biSizeImage)
		return NULL;
	return Buffer[index + mode];
}

int Bitmap::getValueAtXYPosition(uint32_t heightPos, uint32_t widthPos)
{
	int position = (heightPos * bInfoHeader.biWidth + widthPos) * (bInfoHeader.biBitCount / 8);
	if (position < 0 || position > bInfoHeader.biSizeImage)
		return -1;
	return Buffer[position];
}

void Bitmap::setValueAtXYPosition(uint32_t value, uint32_t heightPos, uint32_t widthPos)
{
	int position = (heightPos * bInfoHeader.biWidth + widthPos) * (bInfoHeader.biBitCount / 8);
	if (position >= 0 || position < bInfoHeader.biSizeImage)
	{
		Buffer[position] = value;
	}
}

void Bitmap::setValueAtIndex(uint32_t bufferIndex, uint32_t value)
{
	bufferIndex *= bInfoHeader.biBitCount / 8;
	this->Buffer[bufferIndex] = value;
}

void Bitmap::setTripleAtIndex(RGBTRIPLE colour, uint32_t bufferIndex)
{
	if (bInfoHeader.biBitCount == 24)
	{
		bufferIndex *= 3;
		//if ((int)colour.rgbtBlue > 0)
		//	cout << "here";
		Buffer[bufferIndex] = colour.rgbtBlue;
		colour.rgbtBlue = Buffer[bufferIndex];
		Buffer[bufferIndex + 1] = colour.rgbtGreen;
		Buffer[bufferIndex + 2] = colour.rgbtRed;
	}
}

void Bitmap::copyTripleToIndex(uint32_t bufferIndex, uint32_t bufferIndexNew)
{
	if (bInfoHeader.biBitCount == 24)
	{
		bufferIndex *= 3;
		bufferIndexNew *= 3;
		Buffer[bufferIndex] = Buffer[bufferIndexNew];
		Buffer[bufferIndex + 1] = Buffer[bufferIndexNew + 1];
		Buffer[bufferIndex + 2] = Buffer[bufferIndexNew + 2];
	}
}

void Bitmap::setTripleAtXYPosition(RGBTRIPLE colour, uint32_t heightPos, uint32_t widthPos)
{
	int position = (heightPos * bInfoHeader.biWidth + widthPos) * (bInfoHeader.biBitCount / 8);
	if (position < 0 || position > bInfoHeader.biSizeImage) {
		cerr << "Invalid position" << endl;
		return;
	}
	if (bInfoHeader.biBitCount == 24)
	{
		Buffer[position] = colour.rgbtBlue;
		Buffer[position + 1] = colour.rgbtGreen;
		Buffer[position + 2] = colour.rgbtRed;
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
		bit24Representation = new Bitmap(this->bInfoHeader.biWidth, this->bInfoHeader.biHeight, 24);
	}
	for (uint32_t i = 0; i < this->bInfoHeader.biSizeImage - 2000; i++)
	{
		bit24Representation->setValueAtIndex(i, this->getColourTableEntry(this->getValueAtIndex(i)).rgbtRed);
		bit24Representation->setValueAtIndex(i + 1, this->getColourTableEntry(this->getValueAtIndex(i)).rgbtRed);
		bit24Representation->setValueAtIndex(i + 2, this->getColourTableEntry(this->getValueAtIndex(i)).rgbtRed);
	}
	return bit24Representation;
}

