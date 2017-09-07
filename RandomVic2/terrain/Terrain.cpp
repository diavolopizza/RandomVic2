#include "Terrain.h"
#include "../utils/Random.h"
#include "../utils/BMPHandler.h"


#include <windows.h> //for timeGetTime() mainly
#include <conio.h> //for _getch()

#include <stdio.h> //for printf()
#include <stdlib.h> //for srand()


Terrain::Terrain()
{
}

Terrain::~Terrain()
{
}

BYTE* Terrain::voronoi(int numoflandprov, int width, int height) {
	BYTE* provincebuffer = new BYTE[filesize * 3];
	for (int i = 0; i < filesize * 3; i++)
		provincebuffer[i] = char(0);
	std::cout << "voronoi" << endl;

	vector<int> points;
	vector<vector<int>>colours;

	int r = 0;
	int g = 0;
	int b = 0;
	for (int x = 0; x < 1000; x++)
	{
		if (r % 255 == 0) {
			g++;
			r = 0;
		}
		points.push_back(rando() % filesize);
		vector<int>triple;
		triple.push_back(r++);
		triple.push_back(g);
		triple.push_back(0);
		colours.push_back(triple);
	}
	int counter = 0;
	int distance;
	int closest_point;
	int curpix;
	int point;
	int distancetemp;
	for (int i = 0; i < width; i++) {
		//std::cout << i<<endl;
		for (int curheight = 0; curheight < height; curheight++)
		{
			curpix = i*height + curheight;

			closest_point = 0;
			distance = 999999;

			for (int i = 0; i < points.size(); i++)
			{
				point = points[i];
				//if (point%width > curpix%width) {
				//int pointx = ;
				//int currentx = ;
				//int pointy = ;
				//int currenty = ;
				distancetemp = fabs(sqrt(((point % width - curpix % width) *(point % width - curpix % width)) +
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


BYTE* Terrain::landProvinces(int numoflandprov, int width, int height, Bitmap * terrain, Bitmap * provinceBMP)
{
	BYTE* provincebuffer = new BYTE[filesize * 3];
	for (int i = 0; i < filesize; i++) {
		if (terrain->Buffer[i] == 254) {
			provincebuffer[i * 3] = char(255);
			provincebuffer[i * 3 + 1] = char(255);
			provincebuffer[i * 3 + 2] = char(255);
		}
		else {
			provincebuffer[i * 3] = char(0);
			provincebuffer[i * 3 + 1] = char(0);
			provincebuffer[i * 3 + 2] = char(0);
		}
	}
	cout << "Landprovinces" << endl;
	int provincesize = (filesize + 2000000) / numoflandprov;//better calculation?
	int red = 0;
	int green = 0;
	for (int i = 0; i < numoflandprov; i++)
	{
		int r = 1 + red;
		int g = 1 + green;
		int b = 10;//land gets low blue value
		red++;
		if (red > 254)//if end of colourrange(255) is reached
		{
			green++; //increment second(g) value
			red = 0; //reset red
		}

		vector<int>provincepixels; //pixels of this province
								   //starting point selection
	retry:
		long startingpixel = rando() % filesize;//startingpixel is anywhere in the file
		if (startingpixel >= filewidth && startingpixel <= filesize - filewidth&&provincebuffer[startingpixel * 3] == 0)//then land
		{
			provincebuffer[startingpixel * 3] = b;
			provincebuffer[startingpixel * 3 + 1] = g;
			provincebuffer[startingpixel * 3 + 2] = r;

			provincepixels.push_back(startingpixel);
		}
		else {
			goto retry;
		}
		//province distribution
		for (int x = 0; x < provincesize; x++)
		{
			int curpix = 0;
			while (curpix <= filewidth || curpix >= filesize - filewidth)
			{
				curpix = provincepixels[rando() % provincepixels.size()];
			}
			if (provincebuffer[(curpix + 1) * 3] == 0)
			{
				if ((curpix + 1) % filewidth > curpix % filewidth)
				{
					provincebuffer[(curpix + 1) * 3] = b;
					provincebuffer[(curpix + 1) * 3 + 1] = g;
					provincebuffer[(curpix + 1) * 3 + 2] = r;
					provincepixels.push_back(curpix + 1);
					x++;
				}
			}
			if (provincebuffer[(curpix - 1) * 3] == 0)
			{
				if ((curpix - 1) % filewidth < curpix % filewidth)
				{
					provincebuffer[(curpix - 1) * 3] = b;
					provincebuffer[(curpix - 1) * 3 + 1] = g;
					provincebuffer[(curpix - 1) * 3 + 2] = r;
					provincepixels.push_back(curpix - 1);
					x++;
				}
			}
			if (provincebuffer[(curpix + filewidth) * 3] == 0)
			{
				provincebuffer[(curpix + filewidth) * 3] = b;
				provincebuffer[(curpix + filewidth) * 3 + 1] = g;
				provincebuffer[(curpix + filewidth) * 3 + 2] = r;
				provincepixels.push_back(curpix + filewidth);
				x++;
			}
			if (provincebuffer[(curpix - filewidth) * 3] == 0)
			{
				provincebuffer[(curpix - filewidth) * 3] = b;
				provincebuffer[(curpix - filewidth) * 3 + 1] = g;
				provincebuffer[(curpix - filewidth) * 3 + 2] = r;
				provincepixels.push_back(curpix - filewidth);
				x++;
			}
		}
		Prov* P = new Prov(i, r, g, b, 0); //create new landprovince
										   //hm2[prov_hash(r + g + b) % 2000].push_back(P);
		P->center = provincepixels[0];
		provinces.push_back(P);
	}

	//FILLING START
	cout << "FILLING START" << endl;
	int counterq = 99999999;
	int lastCounter = 9999999999;
	while (counterq > 0 && counterq < lastCounter)
	{
		if (counterq == 0)
			break;
		cout << counterq << endl;
		lastCounter = counterq;
		counterq = 0;
		for (int z = 0; z < filesize; z++)
		{
			if (provincebuffer[z * 3] == 0 /*|| provincebuffer[z * 3] == 253*/)
			{
				counterq++;
				int direction = rando() % 4;
				switch (direction)
				{
				case 0: {
					if (provincebuffer[(z + 1) * 3] != 0 && provincebuffer[(z + 1) * 3] != 255)
					{
						if (z < filesize - 1)
						{
							provincebuffer[z * 3] = provincebuffer[(z + 1) * 3];
							provincebuffer[z * 3 + 1] = provincebuffer[(z + 1) * 3 + 1];
							provincebuffer[z * 3 + 2] = provincebuffer[(z + 1) * 3 + 2];
							//provinces[(provincebuffer[(z + 1) * 3 + 1] - 1) * 255 + provincebuffer[(z + 1) * 3 + 2] - 1]->pixels.insert(z);
						}
					}
					break;
				}
				case 1:
				{
					if (provincebuffer[(z - 1) * 3] != 0 && provincebuffer[(z - 1) * 3] != 255)
					{
						if (z > 0) {
							provincebuffer[z * 3] = provincebuffer[(z - 1) * 3];
							provincebuffer[z * 3 + 1] = provincebuffer[(z - 1) * 3 + 1];
							provincebuffer[z * 3 + 2] = provincebuffer[(z - 1) * 3 + 2];
							//provinces[(provincebuffer[(z - 1) * 3 + 1] - 1) * 255 + provincebuffer[(z - 1) * 3 + 2] - 1]->pixels.insert(z);
						}
					}

					break;
				}
				case 2:
				{
					if (z < filesize - filewidth)
						if (provincebuffer[(z + filewidth) * 3] != 0 && provincebuffer[(z + filewidth) * 3] != 255)
						{
							provincebuffer[z * 3] = provincebuffer[(z + filewidth) * 3];
							provincebuffer[z * 3 + 1] = provincebuffer[(z + filewidth) * 3 + 1];
							provincebuffer[z * 3 + 2] = provincebuffer[(z + filewidth) * 3 + 2];
							//provinces[(provincebuffer[(z + filewidth) * 3 + 1] - 1) * 255 + provincebuffer[(z + filewidth) * 3 + 2] - 1]->pixels.insert(z);

						}
					break;
				}
				case 3:
				{
					if (z > filewidth)
						if (provincebuffer[(z - filewidth) * 3] != 0 && provincebuffer[(z - filewidth) * 3] != 255)
						{
							provincebuffer[z * 3] = provincebuffer[(z - filewidth) * 3];
							provincebuffer[z * 3 + 1] = provincebuffer[(z - filewidth) * 3 + 1];
							provincebuffer[z * 3 + 2] = provincebuffer[(z - filewidth) * 3 + 2];
							//provinces[(provincebuffer[(z - filewidth) * 3 + 1] - 1) * 255 + provincebuffer[(z + -filewidth) * 3 + 2] - 1]->pixels.insert(z);

						}
					break;
				}
				}
			}
		}
	}

	for (int z = 0; z < filesize; z++)
	{
		if (provincebuffer[z * 3] == 0 /*|| provincebuffer[z * 3] == 253*/)
		{
			int distance = 10000;
			Prov* nextOwner = nullptr;
			for (Prov* P : provinces)
			{
				//length of vector between current pixel and province pixel
				int x1 = P->center % filewidth;
				int x2 = z % filewidth;
				int y1 = P->center / fileheight;
				int y2 = z / fileheight;
				if (sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2))) < distance) {
					distance = sqrt(((x1 - x2) *(x1 - x2)) + ((y1 - y2) *(y1 - y2)));
					nextOwner = P;
				}
			}
			provincebuffer[z * 3] = nextOwner->b;
			provincebuffer[z * 3 + 1] = nextOwner->g;
			provincebuffer[z * 3 + 2] = nextOwner->r;
		}
	}
	//set old sea provinces in new province map
	for (int i = 0; i < filesize; i++) {
		if (terrain->Buffer[i] == 254) {
			provincebuffer[i * 3] = provinceBMP->Buffer[i * 3];
			provincebuffer[i * 3 + 1] = provinceBMP->Buffer[i * 3 + 1];
			provincebuffer[i * 3 + 2] = provinceBMP->Buffer[i * 3 + 2];
		}
	}
	return provincebuffer;
}