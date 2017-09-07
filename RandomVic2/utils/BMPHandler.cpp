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
bool BMPHandler::SaveBMP(Bitmap*B, LPCTSTR output, int height, int width, string type)
{
	long paddedsize;
	HANDLE file = CreateFile(output, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);	// now we open the file to write to
	if (file == NULL)
	{
		CloseHandle(file);
		return false;
	}
	unsigned long bwritten;
	if (type == "heightmap")
	{
		B->bitmapinfoheader.biBitCount = 8;
		B->bitmapinfoheader.biHeight = height;
		B->bitmapinfoheader.biWidth = width;
		B->bitmapinfoheader.biSizeImage = width*height;
	}
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

		if (WriteFile(file, B->Buffer, (paddedsize), &bwritten, NULL) == false)
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
		if (WriteFile(file, B->Buffer, (paddedsize), &bwritten, NULL) == false)
		{
			CloseHandle(file);
			return false;
		}
	}
	CloseHandle(file);
	return true;
}


bool BMPHandler::SaveChangedBMP(Bitmap*B, LPCTSTR output)
{
	long paddedsize;
	HANDLE file = CreateFile(output, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);	// now we open the file to write to
	if (file == NULL)
	{
		CloseHandle(file);
		return false;
	}
	unsigned long bwritten;
	//if (type == "provinces")
	//{
	//	B->bitmapinfoheader.biBitCount = 24;
	//	if (B->bitmapinfoheader.biSizeImage<35000000)
	//		B->bitmapinfoheader.biSizeImage *= 3;
	//}
	//else
	//	B->bitmapinfoheader.biBitCount = 8;
	//if (type == "heightmap")
	//{
	//	B->bitmapinfoheader.biBitCount = 8;
	//	B->bitmapinfoheader.biHeight = height;
	//	B->bitmapinfoheader.biWidth = width;
	//	B->bitmapinfoheader.biSizeImage = width*height;
	//	//bitmapinfoheader.biSize = width*height+1078;
	//}
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

		if (WriteFile(file, B->Buffer, (paddedsize), &bwritten, NULL) == false)
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

		if (WriteFile(file, B->Buffer, (paddedsize), &bwritten, NULL) == false)
		{
			CloseHandle(file);
			return false;
		}
	}

	CloseHandle(file);
	return true;
}

Bitmap* BMPHandler::Load24bitBMP(LPCTSTR input, int modifier, string key)
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

	B->Buffer = new BYTE[*size];
	paddedsize = *size;

	SetFilePointer(file, offset, NULL, FILE_BEGIN); //needs to be 58 for copying, but why?
	if (ReadFile(file, B->Buffer, *size, &bytesread, NULL) == false)
	{
		delete[] B->Buffer;
		CloseHandle(file);
		return NULL;
	}

	CloseHandle(file);// everything successful here: close file and return buffer

	bitmaps.insert(pair<string, Bitmap*>(key, B));
}


Bitmap* BMPHandler::Load8bitBMP(LPCTSTR input, int modifier, string key)
{
	Bitmap* B = new Bitmap();




	long* size = new long(0);
	B->colourtable = new BYTE[1024];
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

	/*if (B->bitmapinfoheader.biBitCount == 8)
	{*/
	int offset = 54;
	*size = B->bitmapfileheader.bfSize - offset;	// create buffer to hold the data,-Offsetbits
	B->Buffer = new BYTE[*size];

	SetFilePointer(file, offset, NULL, FILE_BEGIN); //needs to be 58 for copying, but why?
	if (ReadFile(file, B->colourtable, 1024, &bytesread, NULL) == false)
	{
		delete[] B->colourtable;
		CloseHandle(file);
		return NULL;
	}
	//for (int i = 0; i < 1024; i+=4)
	//cout << i/4 << " " << int(B->colourtable[i]) << " " << int(B->colourtable[i+1]) << " " << int(B->colourtable[i+2]) << " " << int(B->colourtable[i + 3]) << endl;
	
	SetFilePointer(file, 1078, NULL, FILE_BEGIN); //needs to be 58 for copying, but why?
	if (ReadFile(file, B->Buffer, *size - 980, &bytesread, NULL) == false)
	{
		delete[]  B->Buffer;
		CloseHandle(file);
		return NULL;
	}
	CloseHandle(file);// everything successful here: close file and return buffer

	bitmaps.insert(pair<string, Bitmap*>(key, B));
}

