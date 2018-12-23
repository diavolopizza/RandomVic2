#include "BMPHandler.h"


BMPHandler::BMPHandler()
{
}


BMPHandler::~BMPHandler()
{
}


Bitmap* BMPHandler::findBitmapByKey(string key) {
	return bitmaps.find(key)->second;
}




//BASIC BITMAP OPERATIONS START
bool BMPHandler::SaveBMPToFile(Bitmap*B, LPCTSTR outputFile)
{
	long paddedsize;
	HANDLE file = CreateFile(outputFile, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);	// now we open the file to write to
	if (file == NULL)
	{
		CloseHandle(file);
		return false;
	}
	unsigned long bwritten;
	if (WriteFile(file, &B->bitmapfileheader, sizeof(BITMAPFILEHEADER), &bwritten, NULL) == false)
	{
		CloseHandle(file);
		return false;
	}
	if (WriteFile(file, &B->bitmapinfoheader, sizeof(BITMAPINFOHEADER), &bwritten, NULL) == false)
	{
		CloseHandle(file);
		return false;
	}
	if (B->bitmapinfoheader.biBitCount == 24)
	{
		paddedsize = B->bitmapinfoheader.biSizeImage;
		if (WriteFile(file, B->getBuffer(), (paddedsize), &bwritten, NULL) == false)
		{
			CloseHandle(file);
			return false;
		}
	}
	else {
		paddedsize = (B->bitmapinfoheader.biWidth)*(B->bitmapinfoheader.biHeight);
		if (WriteFile(file, B->colourtable, 1024, &bwritten, NULL) == false)
		{
			CloseHandle(file);
			return false;
		}
		if (WriteFile(file, B->getBuffer(), (paddedsize), &bwritten, NULL) == false)
		{
			CloseHandle(file);
			return false;
		}
	}
	CloseHandle(file);
	return true;
}

Bitmap * BMPHandler::create24bitBMP(uint32_t width, uint32_t height, string key, uint32_t bitCount)
{
	//BITMAPFILEHEADER    bmfh;  //stores information about the file format
	//BITMAPINFOHEADER    bmih;  //stores information about the bitmap
	//FILE                *file; //stores file pointer

	//						   //create bitmap file header
	//((unsigned char *)&bmfh.bfType)[0] = 'B';
	//((unsigned char *)&bmfh.bfType)[1] = 'M';
	//bmfh.bfSize = 54 + height * width * (bitCount / 8);
	//bmfh.bfReserved1 = 0;
	//bmfh.bfReserved2 = 0;
	//bmfh.bfOffBits = 54;

	////create bitmap information header
	//bmih.biSize = 40;
	//bmih.biWidth = width;
	//bmih.biHeight = height;
	//bmih.biPlanes = 1;
	//bmih.biBitCount = bitCount;
	//bmih.biCompression = 0;
	//bmih.biSizeImage = 0;
	//bmih.biXPelsPerMeter = 3800;
	//bmih.biYPelsPerMeter = 3800;
	//bmih.biClrUsed = 0;
	//bmih.biClrImportant = 0;
	//BYTE* BUFFER = new BYTE[width * height * (bitCount / 8)];
	//Bitmap * B;
	//B->bitmapinfoheader.
	////HBITMAP bitmap = CreateBitmap(width, height, 3, 24, BUFFER);
	return nullptr;
}

Bitmap* BMPHandler::Load24bitBMP(LPCTSTR input, string key)
{
	Bitmap* B = new Bitmap();
	long* size = new long(0);
	long paddedsize;
	DWORD bytesread;	// value to be used in ReadFile funcs

	HANDLE file = CreateFile(input, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);	// open file to read from
	if (NULL == file)
		return NULL; // coudn't open file
	// read file header
	if (ReadFile(file, &B->bitmapfileheader, sizeof(BITMAPFILEHEADER), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		return NULL;
	}
	//read bitmap info
	if (ReadFile(file, &B->bitmapinfoheader, sizeof(BITMAPINFOHEADER), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		return NULL;
	}
	if (ReadFile(file, &B->bitmapinfo, sizeof(BITMAPINFO), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		return NULL;
	}

	// check if bmp is uncompressed
	if (B->bitmapinfoheader.biCompression != BI_RGB)
	{
		CloseHandle(file);
		return NULL;
	}
	int offset = 54;
	*size = B->bitmapinfoheader.biSizeImage;	// create buffer to hold the data,-Offsetbits

	B->setBuffer(new BYTE[*size]);
	paddedsize = *size;

	SetFilePointer(file, offset, NULL, FILE_BEGIN); //needs to be 58 for copying, but why?
	if (ReadFile(file, B->getBuffer(), *size, &bytesread, NULL) == false)
	{
		delete[] B->getBuffer();
		CloseHandle(file);
		return NULL;
	}

	CloseHandle(file);// everything successful here: close file and return buffer
	bitmaps.insert(pair<string, Bitmap*>(key, B));
	return B;
}


Bitmap* BMPHandler::Load8bitBMP(LPCTSTR input, string key)
{
	Bitmap* B = new Bitmap();
	long* size = new long(0);
	B->colourtable = new unsigned char[1024];
	DWORD bytesread;	// value to be used in ReadFile funcs
	HANDLE file = CreateFile(input, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);	// open file to read from
	if (NULL == file)
		return NULL; // coudn't open file
					 // read file header

	if (ReadFile(file, &B->bitmapfileheader, sizeof(BITMAPFILEHEADER), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		return NULL;
	}
	//read bitmap info
	if (ReadFile(file, &B->bitmapinfoheader, sizeof(BITMAPINFOHEADER), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		return NULL;
	}
	if (ReadFile(file, &B->bitmapinfo, sizeof(BITMAPINFO), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		return NULL;
	}

	// check if bmp is uncompressed
	if (B->bitmapinfoheader.biCompression != BI_RGB)
	{
		CloseHandle(file);
		return NULL;
	}
	int offset = 54;
	*size = B->bitmapfileheader.bfSize - 1078;	// create buffer to hold the data,-headerbyte-colourablebyte
	B->setBuffer(new BYTE[*size]);

	SetFilePointer(file, offset, NULL, FILE_BEGIN); //start reading at beginning of colourtable
	if (ReadFile(file, B->colourtable, 1024, &bytesread, NULL) == false)
	{
		delete[] B->colourtable;
		CloseHandle(file);
		return NULL;
	}
	//for (int i = 0; i < 1024; i+=4)
	//cout << i/4 << " " << int(B->colourtable[i]) << " " << int(B->colourtable[i+1]) << " " << int(B->colourtable[i+2]) << " " << int(B->colourtable[i + 3]) << endl;

	SetFilePointer(file, 1078, NULL, FILE_BEGIN); //start reading after end of colourtable
	if (ReadFile(file, B->getBuffer(), *size, &bytesread, NULL) == false)
	{
		delete[]  B->getBuffer();
		CloseHandle(file);
		return NULL;
	}
	CloseHandle(file);// everything successful here: close file and return buffer

	bitmaps.insert(pair<string, Bitmap*>(key, B));
	return B;
}

