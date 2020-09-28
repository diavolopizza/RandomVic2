#include "BMPHandler.h"


BMPHandler::BMPHandler()
{
}


BMPHandler::~BMPHandler()
{
}


Bitmap BMPHandler::findBitmapByKey(string key) {
	return bitmaps.find(key)->second;
}




//BASIC BITMAP OPERATIONS START
bool BMPHandler::SaveBMPToFile(const Bitmap B, LPCTSTR outputFile)
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
	if (WriteFile(file, &B.bFileHeader, sizeof(BITMAPFILEHEADER), &bwritten, NULL) == false)
	{
		CloseHandle(file);
		return false;
	}
	if (WriteFile(file, &B.bInfoHeader, sizeof(BITMAPINFOHEADER), &bwritten, NULL) == false)
	{
		CloseHandle(file);
		return false;
	}
	if (B.bInfoHeader.biBitCount == 24)
	{
		paddedsize = B.bInfoHeader.biSizeImage;
		if (WriteFile(file, B.getBuffer(), (paddedsize), &bwritten, NULL) == false)
		{
			CloseHandle(file);
			return false;
		}
	}
	else {
		paddedsize = (B.bInfoHeader.biWidth)*(B.bInfoHeader.biHeight);
		if (WriteFile(file, B.colourtable, 1024, &bwritten, NULL) == false)
		{
			CloseHandle(file);
			return false;
		}
		if (WriteFile(file, B.getBuffer(), (paddedsize), &bwritten, NULL) == false)
		{
			CloseHandle(file);
			return false;
		}
	}
	CloseHandle(file);
	return true;
}

Bitmap BMPHandler::Load24bitBMP(LPCTSTR input, string key)
{
	Bitmap B;
	long* size = new long(0);
	long paddedsize;
	DWORD bytesread;	// value to be used in ReadFile funcs

	HANDLE file = CreateFile(input, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);	// open file to read from
	if (NULL == file)
		return B; // coudn't open file
	// read file header
	if (ReadFile(file, &B.bFileHeader, sizeof(BITMAPFILEHEADER), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		//return NULL;
	}
	//read bitmap info
	if (ReadFile(file, &B.bInfoHeader, sizeof(BITMAPINFOHEADER), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		//return NULL;
	}
	if (ReadFile(file, &B.bInfo, sizeof(BITMAPINFO), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		//return NULL;
	}

	// check if bmp is uncompressed
	if (B.bInfoHeader.biCompression != BI_RGB)
	{
		CloseHandle(file);
		//return NULL;
	}
	int offset = 54;
	*size = B.bInfoHeader.biSizeImage;	// create buffer to hold the data,-Offsetbits

	B.setBuffer(new BYTE[*size]);
	paddedsize = *size;

	SetFilePointer(file, offset, NULL, FILE_BEGIN); //needs to be 58 for copying, but why?
	if (ReadFile(file, B.getBuffer(), *size, &bytesread, NULL) == false)
	{
		//delete[] B.getBuffer();
		CloseHandle(file);
		//return NULL;
	}

	CloseHandle(file);// everything successful here: close file and return buffer
	B.setIndexFactor(3u);
	bitmaps.insert(pair<string, Bitmap>(key, B));
	return B;
}


Bitmap BMPHandler::Load8bitBMP(LPCTSTR input, string key)
{
	Bitmap B;
	long* size = new long(0);
	B.colourtable = new unsigned char[1024];
	DWORD bytesread;	// value to be used in ReadFile funcs
	HANDLE file = CreateFile(input, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);	// open file to read from
	if (NULL == file)
		return Bitmap(); // coudn't open file
					 // read file header

	if (ReadFile(file, &B.bFileHeader, sizeof(BITMAPFILEHEADER), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		return Bitmap();
	}
	//read bitmap info
	if (ReadFile(file, &B.bInfoHeader, sizeof(BITMAPINFOHEADER), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		return Bitmap();
	}
	if (ReadFile(file, &B.bInfo, sizeof(BITMAPINFO), &bytesread, NULL) == false)
	{
		CloseHandle(file);
		return Bitmap();
	}

	// check if bmp is uncompressed
	if (B.bInfoHeader.biCompression != BI_RGB)
	{
		CloseHandle(file);
		return Bitmap();
	}
	int offset = 54;
	*size = B.bFileHeader.bfSize - 1078;	// create buffer to hold the data,-headerbyte-colourablebyte
	B.setBuffer(new BYTE[*size]);

	SetFilePointer(file, offset, NULL, FILE_BEGIN); //start reading at beginning of colourtable
	if (ReadFile(file, B.colourtable, 1024, &bytesread, NULL) == false)
	{
		//delete[] B.colourtable;
		CloseHandle(file);
		return Bitmap();
	}
	//for (int i = 0; i < 1024; i+=4)
	//cout << i/4 << " " << int(B->colourtable[i]) << " " << int(B->colourtable[i+1]) << " " << int(B->colourtable[i+2]) << " " << int(B->colourtable[i + 3]) << endl;

	SetFilePointer(file, 1078, NULL, FILE_BEGIN); //start reading after end of colourtable
	if (ReadFile(file, B.getBuffer(), *size, &bytesread, NULL) == false)
	{
		//delete[]  B.getBuffer();
		CloseHandle(file);
		return Bitmap();
	}
	CloseHandle(file);// everything successful here: close file and return buffer

	B.setIndexFactor(1u);
	bitmaps.insert(pair<string, Bitmap>(key, B));
	return B;
}

