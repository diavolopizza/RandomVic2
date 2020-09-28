#include "Bitmap.h"
#include <iostream>


Bitmap::Bitmap()
{
}

Bitmap::Bitmap(uint32_t width, uint32_t height, uint32_t bitCount)
{
	//create bitmap file header
	((BYTE*)&bFileHeader.bfType)[0] = 'B';
	((BYTE*)&bFileHeader.bfType)[1] = 'M';
	bFileHeader.bfReserved1 = 0;
	bFileHeader.bfReserved2 = 0;

	//create bitmap information header
	bInfoHeader.biSize = 40;
	bInfoHeader.biPlanes = 1;
	bInfoHeader.biCompression = 0;
	bInfoHeader.biXPelsPerMeter = 3800;
	bInfoHeader.biYPelsPerMeter = 3800;

	if (bitCount == 24) {

		indexFactor = 3;
		bFileHeader.bfOffBits = 54;
		this->bFileHeader.bfSize = 54 + height * width * indexFactor;
		this->bInfoHeader.biWidth = width;
		this->bInfoHeader.biHeight = height;
		this->bInfoHeader.biSizeImage = width * height*indexFactor;
		this->Buffer = std::shared_ptr<BYTE>(new unsigned char[width*height*indexFactor]);
		for (uint32_t i = 0; i < width*height*indexFactor; i++)
		{
			Buffer.get()[i] = 0;
		}
		this->bInfoHeader.biBitCount = 24;
		bInfo.bmiHeader = bInfoHeader;
	}
	else {
		indexFactor = 1;
		bFileHeader.bfOffBits = 54 + 256 * 4;
		this->bFileHeader.bfSize = 54 + 256 * 4 + height * width;
		this->bInfoHeader.biWidth = width;
		this->bInfoHeader.biHeight = height;
		this->bInfoHeader.biSizeImage = width * height;
		this->Buffer = std::shared_ptr<BYTE>(new unsigned char[width*height]);
		for (uint32_t i = 0; i < width*height; i++)
		{
			Buffer.get()[i] = 0;
		}
		bInfo.bmiHeader = bInfoHeader;
	}
}

Bitmap::Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, BYTE* buffer) : Bitmap(width, height, bitCount)
{
	this->Buffer = std::shared_ptr<BYTE>(buffer);
}

Bitmap::Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, Bitmap bitmap) : Bitmap(width, height, bitCount)
{
	//this->Buffer = std::shared_ptr<BYTE>(bitmap.Buffer);
	this->Buffer = bitmap.Buffer;
	
}


Bitmap::~Bitmap()
{
	//if (this->Buffer != nullptr)
	//delete Buffer;
}

RGBTRIPLE Bitmap::getColourTableEntry(uint32_t index) const
{
	RGBTRIPLE colour{ colourtable[index * 4], colourtable[index * 4 + 1], colourtable[index * 4 + 2] };
	return colour;
}

RGBTRIPLE Bitmap::getTripleAtIndex(const uint32_t bufIndex) const
{
	return RGBTRIPLE{ Buffer.get()[bufIndex*indexFactor],Buffer.get()[bufIndex*indexFactor + 1], Buffer.get()[bufIndex*indexFactor + 2] };
}

void Bitmap::setBitmapSize(const uint32_t width, const uint32_t height)
{
	if (this->bInfoHeader.biBitCount == 24) {
		this->bFileHeader.bfSize = 54 + height * width * (bInfoHeader.biBitCount / 8);
		this->bInfoHeader.biWidth = width;
		this->bInfoHeader.biHeight = height;
		this->bInfoHeader.biSizeImage = width * height*(bInfoHeader.biBitCount / 8);
		this->Buffer = std::shared_ptr<BYTE>(new unsigned char[width*height*(bInfoHeader.biBitCount / 8)]);
	}
	else {
		this->bFileHeader.bfSize = 54 + 256 * 4 + height * width * (bInfoHeader.biBitCount / 8);
		this->bInfoHeader.biWidth = width;
		this->bInfoHeader.biHeight = height;
		this->bInfoHeader.biSizeImage = width * height*(bInfoHeader.biBitCount / 8);
		this->Buffer = std::shared_ptr<BYTE>(new unsigned char[width*height*(bInfoHeader.biBitCount / 8)]);
	}
}

unsigned char Bitmap::getValueAtIndex(const uint32_t index, const uint32_t mode) const
{
	if (index * indexFactor < this->bInfoHeader.biSizeImage)
		return Buffer.get()[index * indexFactor + mode];
	return 0;
}

unsigned char Bitmap::getValueAtXYPosition(const uint32_t heightPos, const uint32_t widthPos) const
{
	auto position = (heightPos * bInfoHeader.biWidth + widthPos) * indexFactor;
	if (position < bInfoHeader.biSizeImage)
		return Buffer.get()[position];
	return 0;
}

void Bitmap::setValueAtXYPosition(const unsigned char value, const uint32_t heightPos, const uint32_t widthPos)
{
	auto position = (heightPos * bInfoHeader.biWidth + widthPos) * indexFactor;
	if (position < bInfoHeader.biSizeImage)
		Buffer.get()[position] = value;
}

void Bitmap::setValueAtIndex(const uint32_t bufIndex, const unsigned char value)
{
	this->Buffer.get()[bufIndex*indexFactor] = value;
}

void Bitmap::setTripleAtIndex(const RGBTRIPLE colour, uint32_t bufIndex)
{
	bufIndex *= indexFactor;
	Buffer.get()[bufIndex] = colour.rgbtBlue;
	Buffer.get()[bufIndex + 1] = colour.rgbtGreen;
	Buffer.get()[bufIndex + 2] = colour.rgbtRed;
}

void Bitmap::copyTripleToIndex(uint32_t bufIndexTo, uint32_t bufIndexFrom)
{
	bufIndexTo *= indexFactor;
	bufIndexFrom *= indexFactor;
	Buffer.get()[bufIndexTo] = Buffer.get()[bufIndexFrom];
	Buffer.get()[bufIndexTo + 1] = Buffer.get()[bufIndexFrom + 1];
	Buffer.get()[bufIndexTo + 2] = Buffer.get()[bufIndexFrom + 2];
}

void Bitmap::setTripleAtXYPosition(const RGBTRIPLE colour, const uint32_t heightPos, const uint32_t widthPos)
{
	auto position = (heightPos * bInfoHeader.biWidth + widthPos) * indexFactor;
	if (position < bInfoHeader.biSizeImage) {
		Buffer.get()[position] = colour.rgbtBlue;
		Buffer.get()[position + 1] = colour.rgbtGreen;
		Buffer.get()[position + 2] = colour.rgbtRed;
	}
}

void Bitmap::setBuffer(std::shared_ptr<BYTE> Buffer)
{
	//std::shared_ptr<BYTE> up1(Buffer);
	this->Buffer = Buffer;
}

void Bitmap::setBuffer(BYTE* Buffer)
{
	this->Buffer = std::shared_ptr<BYTE>(Buffer);
}

void Bitmap::setIndexFactor(const uint32_t indexFactor)
{
	this->indexFactor = indexFactor;
}

BYTE* Bitmap::getBuffer() const
{
	return this->Buffer.get();
}

//Bitmap Bitmap::get24BitRepresentation()
//{
//	if (bit24Representation == nullptr)
//	{
//		bit24Representation(this->bInfoHeader.biWidth, this->bInfoHeader.biHeight, 24);
//	}
//	for (uint32_t i = 0; i < this->bInfoHeader.biSizeImage - 2000; i++)
//	{
//		bit24Representation->setValueAtIndex(i, this->getColourTableEntry(this->getValueAtIndex(i)).rgbtRed);
//		bit24Representation->setValueAtIndex(i + 1, this->getColourTableEntry(this->getValueAtIndex(i)).rgbtRed);
//		bit24Representation->setValueAtIndex(i + 2, this->getColourTableEntry(this->getValueAtIndex(i)).rgbtRed);
//	}
//	return bit24Representation;
//}

