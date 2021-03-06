#include "Bitmap.h"


Bitmap::Bitmap()
{
	this->colourtable = vector<BYTE>(1024);
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
		this->Buffer = vector<BYTE>(width*height*indexFactor);
		//for (uint32_t i = 0; i < width*height*indexFactor; i++)
		//{
		//	Buffer[i] = 0;
		//}
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
		this->colourtable = vector<BYTE>(1024);
		this->Buffer = vector<BYTE>(width*height);
		for (uint32_t i = 0; i < 1024; i++)
		{
			colourtable[i] = i % 256;
		}
		for (uint32_t i = 0; i < width*height; i++)
		{
			Buffer[i] = 0;
		}
		bInfo.bmiHeader = bInfoHeader;
	}
}

Bitmap::Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, vector<BYTE> buffer) : Bitmap(width, height, bitCount)
{
	this->Buffer = buffer;
}

Bitmap::Bitmap(uint32_t width, uint32_t height, uint32_t bitCount, Bitmap bitmap) : Bitmap(width, height, bitCount)
{
	this->Buffer = bitmap.Buffer;
}


Bitmap::~Bitmap()
{
}

RGBTRIPLE Bitmap::getColourTableEntry(uint32_t index) const
{
	RGBTRIPLE colour{ colourtable[index * 4], colourtable[index * 4 + 1], colourtable[index * 4 + 2] };
	return colour;
}

RGBTRIPLE Bitmap::getTripleAtIndex(const uint32_t bufIndex) const
{
	return RGBTRIPLE{ Buffer[bufIndex*indexFactor],Buffer[bufIndex*indexFactor + 1], Buffer[bufIndex*indexFactor + 2] };
}

void Bitmap::setBitmapSize(const uint32_t width, const uint32_t height)
{
	if (this->bInfoHeader.biBitCount == 24) {
		this->bFileHeader.bfSize = 54 + height * width * (bInfoHeader.biBitCount / 8);
		this->bInfoHeader.biWidth = width;
		this->bInfoHeader.biHeight = height;
		this->bInfoHeader.biSizeImage = width * height*(bInfoHeader.biBitCount / 8);
		this->Buffer = vector<BYTE>(width*height*(bInfoHeader.biBitCount / 8));
	}
	else {
		this->bFileHeader.bfSize = 54 + 256 * 4 + height * width * (bInfoHeader.biBitCount / 8);
		this->bInfoHeader.biWidth = width;
		this->bInfoHeader.biHeight = height;
		this->bInfoHeader.biSizeImage = width * height*(bInfoHeader.biBitCount / 8);
		this->Buffer = vector<BYTE>(width*height*(bInfoHeader.biBitCount / 8));
	}
}

unsigned char Bitmap::getValueAtIndex(const uint32_t index, const uint32_t mode) const noexcept
{
	//cout << "Getting " << index << endl;
	if (index < bInfoHeader.biSizeImage)
		return index * indexFactor < this->bInfoHeader.biSizeImage ? Buffer[index * indexFactor + mode] : 0;
}

unsigned char Bitmap::getValueAtXYPosition(const uint32_t heightPos, const uint32_t widthPos) const
{
	auto position = (heightPos * bInfoHeader.biWidth + widthPos) * indexFactor;
	if (position < bInfoHeader.biSizeImage)
		return Buffer[position];
	return 0;
}

void Bitmap::setValueAtXYPosition(const unsigned char value, const uint32_t heightPos, const uint32_t widthPos)
{
	auto position = (heightPos * bInfoHeader.biWidth + widthPos) * indexFactor;
	if (position < bInfoHeader.biSizeImage)
		Buffer[position] = value;
}

void Bitmap::setValueAtIndex(const uint32_t bufIndex, const unsigned char value)
{
	if (bufIndex < bInfoHeader.biSizeImage)
		this->Buffer[bufIndex*indexFactor] = value;
}

void Bitmap::setTripleAtIndex(const RGBTRIPLE colour, uint32_t bufIndex)
{
	bufIndex *= indexFactor;
	Buffer[bufIndex] = colour.rgbtBlue;
	Buffer[bufIndex + 1] = colour.rgbtGreen;
	Buffer[bufIndex + 2] = colour.rgbtRed;
}

void Bitmap::copyTripleToIndex(uint32_t bufIndexTo, uint32_t bufIndexFrom)
{
	bufIndexTo *= indexFactor;
	bufIndexFrom *= indexFactor;
	Buffer[bufIndexTo] = Buffer[bufIndexFrom];
	Buffer[bufIndexTo + 1] = Buffer[bufIndexFrom + 1];
	Buffer[bufIndexTo + 2] = Buffer[bufIndexFrom + 2];
}

void Bitmap::setTripleAtXYPosition(const RGBTRIPLE colour, const uint32_t heightPos, const uint32_t widthPos)
{
	auto position = (heightPos * bInfoHeader.biWidth + widthPos) * indexFactor;
	if (position < bInfoHeader.biSizeImage) {
		Buffer[position] = colour.rgbtBlue;
		Buffer[position + 1] = colour.rgbtGreen;
		Buffer[position + 2] = colour.rgbtRed;
	}
}

void Bitmap::setBuffer(vector<BYTE> Buffer)
{
	this->Buffer = Buffer;
}

void Bitmap::setIndexFactor(const uint32_t indexFactor)
{
	this->indexFactor = indexFactor;
}

const vector<BYTE>& Bitmap::getBuffer() const
{
	return this->Buffer;
}

vector<BYTE>& Bitmap::getColourtable()
{
	return colourtable;
}

const vector<BYTE>& Bitmap::getArea(uint32_t center, uint32_t width, uint32_t height, vector<BYTE> &buff) const
{
	int offset = center / 3 + (width / 2) - (height / 2) * this->bInfoHeader.biWidth;// center - (width / 2) - (width * (height / 2));
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			//cout << (offset + (h*this->bInfoHeader.biWidth + w)) * indexFactor << endl;
			buff[(h*width + w) * indexFactor] = this->Buffer[(offset + (h*this->bInfoHeader.biWidth + w)) * indexFactor];
			buff[(h*width + w) * indexFactor + 1] = this->Buffer[((offset + (h*this->bInfoHeader.biWidth + w)) * indexFactor) + 1];
			buff[(h*width + w) * indexFactor + 2] = this->Buffer[((offset + (h*this->bInfoHeader.biWidth + w)) * indexFactor) + 2];
		}
	}
	return buff;
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

