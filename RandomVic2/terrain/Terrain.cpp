#include "Terrain.h"
#include "../utils/Random.h"
#include "../utils/BMPHandler.h"
#include <windows.h> //for timeGetTime() mainly
//MACROS
#define LEFT(val) \
(val-1)
#define RIGHT(val) \
(val+1)
#define ABOVE(val, offset) \
(val+offset)
#define BELOW(val, offset) \
(val-offset)

Terrain::Terrain(ranlux48* random)
{
	this->random = random;
}

Terrain::~Terrain()
{
}

BYTE* Terrain::voronoi(uint32_t numoflandprov, uint32_t width, uint32_t height) {

	uint32_t filesize = width*height;
	BYTE* provincebuffer = new BYTE[filesize * 3];
	for (uint32_t i = 0; i < filesize * 3; i++)
		provincebuffer[i] = char(0);
	std::cout << "voronoi" << endl;

	vector<int> points;
	vector<vector<int>>colours;

	uint32_t r = 0;
	uint32_t g = 0;
	uint32_t b = 0;
	for (uint32_t x = 0; x < 1000; x++)
	{
		if (r % 255 == 0) {
			g++;
			r = 0;
		}
		points.push_back((*random)() % filesize);
		vector<int>triple;
		triple.push_back(r++);
		triple.push_back(g);
		triple.push_back(0);
		colours.push_back(triple);
	}
	uint32_t counter = 0;
	uint32_t distance;
	uint32_t closest_point;
	uint32_t curpix;
	uint32_t point;
	uint32_t distancetemp;
	for (uint32_t i = 0; i < width; i++) {
		//std::cout << i<<endl;
		for (uint32_t curheight = 0; curheight < height; curheight++)
		{
			curpix = i*height + curheight;

			closest_point = 0;
			distance = 999999;

			for (uint32_t i = 0; i < points.size(); i++)
			{
				point = points[i];
				//if (point%width > curpix%width) {
				//int pointx = ;
				//int currentx = ;
				//int pointy = ;
				//int currenty = ;
				distancetemp = (uint32_t)fabs(sqrt(((point % width - curpix % width) *(point % width - curpix % width)) +
					((point / height - curpix / height) *(point / height - curpix / height))));
				if (distancetemp < distance)
				{
					distance = distancetemp;
					closest_point = i;
				}
				//}




				provincebuffer[(curpix) * 3] = colours[closest_point][2];
				provincebuffer[(curpix) * 3 + 1] = colours[closest_point][1];
				provincebuffer[(curpix) * 3 + 2] = colours[closest_point][0];
			}
		}
	}


	//float xValues[4] = { -22, -17, 4,22 };
	//float yValues[4] = { -9, 31,13,-5 };

	//long count = 4;

	//VoronoiDiagramGenerator vdg;
	//vdg.generateVoronoi(xValues, yValues, count, -100, 100, -100, 100, 3);

	//vdg.resetIterator();

	//float x1, y1, x2, y2;

	//printf("\n-------------------------------\n");
	//while (vdg.getNext(x1, y1, x2, y2))
	//{
	//	printf("GOT Line (%f,%f)->(%f,%f)\n", x1, y1, x2, y2);

	//}

	//return 0;




	return provincebuffer;
}

void Terrain::determineStartingPixel(uint32_t bmpSize, uint32_t bmpWidth, BYTE* provinceBuffer, vector<uint32_t> &provincePixels, RGBTRIPLE provinceColour) {
	uint32_t startingPixel = (*random)() % bmpSize;//startingpixel is anywhere in the file
	while (!(startingPixel >= bmpWidth && startingPixel <= bmpSize - bmpWidth && provinceBuffer[startingPixel * 3] == 0))
	{
		startingPixel = (*random)() % bmpSize;//startingpixel is anywhere in the file
	}
	assignColour(provinceColour, provinceBuffer, startingPixel);
	provincePixels.push_back(startingPixel);
}

void Terrain::assignColour(RGBTRIPLE provinceColour, BYTE* provinceBuffer, uint32_t pixel) {
	provinceBuffer[pixel * 3] = provinceColour.rgbtBlue;
	provinceBuffer[pixel * 3 + 1] = provinceColour.rgbtGreen;
	provinceBuffer[pixel * 3 + 2] = provinceColour.rgbtRed;
}

BYTE* Terrain::landProvinces(uint32_t numoflandprov, Bitmap * terrainBMP, Bitmap * provinceBMP)
{
	uint32_t bmpWidth = terrainBMP->bitmapinfoheader.biWidth;
	uint32_t bmpHeight = terrainBMP->bitmapinfoheader.biHeight;
	uint32_t bmpSize = bmpWidth*bmpHeight;
	BYTE* provinceBuffer = new BYTE[bmpSize * 3];
	for (uint32_t i = 0; i < bmpSize; i++) {
		if (terrainBMP->Buffer[i] == 254) {
			provinceBuffer[i * 3] = char(255);
			provinceBuffer[i * 3 + 1] = char(255);
			provinceBuffer[i * 3 + 2] = char(255);
		}
		else {
			provinceBuffer[i * 3] = char(0);
			provinceBuffer[i * 3 + 1] = char(0);
			provinceBuffer[i * 3 + 2] = char(0);
		}
	}
	cout << "Landprovinces" << endl;
	uint32_t provincesize = bmpSize / numoflandprov;//better calculation?
	uint32_t red = 0;
	uint32_t green = 0;
	for (uint32_t i = 0; i < numoflandprov; i++)
	{
		RGBTRIPLE provinceColour;
		provinceColour.rgbtRed = 1 + red;
		provinceColour.rgbtGreen = 1 + green;
		provinceColour.rgbtBlue = 10;//land gets low blue value
		red++;
		if (red > 254)//if end of colourrange(255) is reached
		{
			green++; //increment second(g) value
			red = 0; //reset red
		}

		vector<uint32_t>provincePixels; //pixels of this province
								   //starting point selection
		determineStartingPixel(bmpSize, bmpWidth, provinceBuffer, provincePixels, provinceColour);
		//province distribution
		for (uint32_t x = 0; x < provincesize; x++)
		{
			uint32_t currentPixel = 0;
			while (currentPixel <= bmpWidth || currentPixel >= bmpSize - bmpWidth)
			{
				currentPixel = provincePixels[(*random)() % provincePixels.size()];
			}
			if (provinceBuffer[RIGHT(currentPixel) * 3] == 0)
			{
				if (RIGHT(currentPixel) % bmpWidth > currentPixel % bmpWidth)
				{
					assignColour(provinceColour, provinceBuffer, RIGHT(currentPixel));
					provincePixels.push_back(RIGHT(currentPixel));
					x++;
				}
			}
			if (provinceBuffer[LEFT(currentPixel) * 3] == 0)
			{
				if (LEFT(currentPixel) % bmpWidth < currentPixel % bmpWidth)
				{
					assignColour(provinceColour, provinceBuffer, LEFT(currentPixel));
					provincePixels.push_back(LEFT(currentPixel));
					x++;
				}
			}
			if (provinceBuffer[ABOVE(currentPixel, bmpWidth) * 3] == 0)
			{
				assignColour(provinceColour, provinceBuffer, ABOVE(currentPixel, bmpWidth));
				provincePixels.push_back(ABOVE(currentPixel, bmpWidth));
				x++;
			}
			if (provinceBuffer[BELOW(currentPixel, bmpWidth) * 3] == 0)
			{
				assignColour(provinceColour, provinceBuffer, BELOW(currentPixel, bmpWidth));
				provincePixels.push_back(BELOW(currentPixel, bmpWidth));
				x++;
			}
		}
		Prov* P = new Prov(i, provinceColour, 0); //create new landprovince
		P->center = provincePixels[0];
		provinces.push_back(P);
	}

	//FILLING START
	cout << "FILLING START" << endl;
	uint32_t unassignedPixels = 99999999;
	uint32_t previousUnassignedPixels = 5*10^5;
	while (unassignedPixels > 0 && unassignedPixels < previousUnassignedPixels)
	{
		if (unassignedPixels == 0)
			break;
		cout << unassignedPixels << endl;
		previousUnassignedPixels = unassignedPixels;
		unassignedPixels = 0;
		for (uint32_t unassignedPixel = 0; unassignedPixel < bmpSize; unassignedPixel++)
		{
			if (provinceBuffer[unassignedPixel * 3] == 0 /*|| provincebuffer[z * 3] == 253*/)
			{
				unassignedPixels++;
				uint32_t direction = (*random)() % 4;
				switch (direction)
				{
				case 0: {
					if (provinceBuffer[RIGHT(unassignedPixel) * 3] != 0 && provinceBuffer[RIGHT(unassignedPixel) * 3] != 255 && unassignedPixel < bmpSize - 1)
					{
						provinceBuffer[unassignedPixel * 3] = provinceBuffer[RIGHT(unassignedPixel) * 3];
						provinceBuffer[unassignedPixel * 3 + 1] = provinceBuffer[RIGHT(unassignedPixel) * 3 + 1];
						provinceBuffer[unassignedPixel * 3 + 2] = provinceBuffer[RIGHT(unassignedPixel) * 3 + 2];
						//provinces[(provincebuffer[(z + 1) * 3 + 1] - 1) * 255 + provincebuffer[(z + 1) * 3 + 2] - 1]->pixels.insert(z);
					}
					break;
				}
				case 1:
				{
					if (provinceBuffer[LEFT(unassignedPixel) * 3] != 0 && provinceBuffer[LEFT(unassignedPixel) * 3] != 255 && unassignedPixel > 0)
					{
						provinceBuffer[unassignedPixel * 3] = provinceBuffer[LEFT(unassignedPixel) * 3];
						provinceBuffer[unassignedPixel * 3 + 1] = provinceBuffer[LEFT(unassignedPixel) * 3 + 1];
						provinceBuffer[unassignedPixel * 3 + 2] = provinceBuffer[LEFT(unassignedPixel) * 3 + 2];
						//provinces[(provincebuffer[(z - 1) * 3 + 1] - 1) * 255 + provincebuffer[(z - 1) * 3 + 2] - 1]->pixels.insert(z);
					}
					break;
				}
				case 2:
				{
					if (unassignedPixel < bmpSize - bmpWidth && provinceBuffer[ABOVE(unassignedPixel, bmpWidth) * 3] != 0 && provinceBuffer[ABOVE(unassignedPixel, bmpWidth) * 3] != 255)
					{
						provinceBuffer[unassignedPixel * 3] = provinceBuffer[ABOVE(unassignedPixel, bmpWidth) * 3];
						provinceBuffer[unassignedPixel * 3 + 1] = provinceBuffer[ABOVE(unassignedPixel, bmpWidth) * 3 + 1];
						provinceBuffer[unassignedPixel * 3 + 2] = provinceBuffer[ABOVE(unassignedPixel, bmpWidth) * 3 + 2];
						//provinces[(provincebuffer[(z + filewidth) * 3 + 1] - 1) * 255 + provincebuffer[(z + filewidth) * 3 + 2] - 1]->pixels.insert(z);
					}
					break;
				}
				case 3:
				{
					if (unassignedPixel > bmpWidth && provinceBuffer[BELOW(unassignedPixel, bmpWidth) * 3] != 0 && provinceBuffer[BELOW(unassignedPixel, bmpWidth) * 3] != 255)
					{
						provinceBuffer[unassignedPixel * 3] = provinceBuffer[BELOW(unassignedPixel, bmpWidth) * 3];
						provinceBuffer[unassignedPixel * 3 + 1] = provinceBuffer[BELOW(unassignedPixel, bmpWidth) * 3 + 1];
						provinceBuffer[unassignedPixel * 3 + 2] = provinceBuffer[BELOW(unassignedPixel, bmpWidth) * 3 + 2];
						//provinces[(provincebuffer[(z - filewidth) * 3 + 1] - 1) * 255 + provincebuffer[(z + -filewidth) * 3 + 2] - 1]->pixels.insert(z);
					}
					break;
				}
				}
			}
		}
	}
	assignRemainingPixels(bmpSize, provinceBuffer, provinces, bmpWidth, bmpHeight);
	//set old sea provinces in new province map
	for (uint32_t i = 0; i < bmpSize; i++) {
		if (terrainBMP->Buffer[i] == 254) {
			//provincebuffer[i * 3] = provinceBMP->Buffer[i * 3];
			provinceBuffer[i * 3 + 1] = provinceBMP->Buffer[i * 3 + 1];
			provinceBuffer[i * 3 + 2] = provinceBMP->Buffer[i * 3 + 2];
		}
	}
	return provinceBuffer;
}

void Terrain::assignRemainingPixels(uint32_t bmpSize, BYTE* provinceBuffer, vector<Prov*> provinces, uint32_t bmpWidth, uint32_t bmpHeight) {
	//Assign remaining pixels
	for (uint32_t unassignedPixel = 0; unassignedPixel < bmpSize; unassignedPixel++)
	{
		if (provinceBuffer[unassignedPixel * 3] == 0 /*|| provincebuffer[z * 3] == 253*/)
		{
			uint32_t distance = 10000;
			Prov* nextOwner = nullptr;
			for (Prov* P : provinces)
			{
				//length of vector between current pixel and province pixel
				uint32_t x1 = P->center % bmpWidth;
				uint32_t x2 = unassignedPixel % bmpWidth;
				uint32_t y1 = P->center / bmpHeight;
				uint32_t y2 = unassignedPixel / bmpHeight;
				if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
					distance = (uint32_t)sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
					nextOwner = P;
				}
			}
			assignColour(nextOwner->colour, provinceBuffer, unassignedPixel);
		}
	}
}