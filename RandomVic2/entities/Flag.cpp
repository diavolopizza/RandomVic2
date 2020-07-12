#include "Flag.h"


Flag::Flag(ranlux48* random)
{
	this->random = random;

	// load the template
	unsigned char* targaimage; 
	targaimage = (unsigned char*)tga_load("C:\\Users\\Paul\\Documents\\Visual Studio 2017\\Projects\\\RandomVic2\\RandomVic2\\resources\\gfx\\flags\\template.tga", &width, &height, TGA_TRUECOLOR_32);
	this->flag = targaimage;

	int type = 4;// (*random)() % 5;


	colours = generateColours();
	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			switch (type)
			{
			case 0:
			{
				flagType = TRICOLORE;
				tricolore(i, j);
				break;
			}
			case 1:
			{
				flagType = ROTATEDTRICOLORE;
				rotatedTricolore(i, j);
				break;
			}
			case 2:
			{
				flagType = SQUARE;
				squareSquared(i, j);
				break;
			}
			case 3:
			{
				flagType = CIRCLE;
				circle(i, j);
				break;
			}
			case 4:
			{
				flagType = MOON;
				halfMoon(i, j);
				break;
			}
			default:
				break;
			}
		}
	}
}

Flag::~Flag()
{
}

void Flag::tricolore(int i, int j)
{
	unsigned short colourIndex = j / (width / 3);
	setPixel(colours[colourIndex], i, j);
}

void Flag::rotatedTricolore(int i, int j)
{
	unsigned short colourIndex = i / (height / 3);
	setPixel(colours[colourIndex], i, j);
}

void Flag::squareSquared(int i, int j)
{
	unsigned short colourIndex = 0;
	setPixel(colours[colourIndex], i, j);
	if (abs((int)j - width / 2) < 16 && abs((int)i - height / 2) < 16)
		setPixel(colours[1], i, j);
}

void Flag::circle(int i, int j)
{
	struct Point { int x; int y; };
	Point center{ width / 2, height / 2 };
	unsigned short colourIndex = 0;
	setPixel(colours[colourIndex], i, j);
	Point curPos{ j, i };
	int distance = abs(std::hypot(center.x - curPos.x, center.y - curPos.y));
	if (distance < 15) {
		setPixel(colours[1], i, j);
	}
}

void Flag::halfMoon(int i, int j)
{
	float radius = 15;
	struct Point { int x; int y; };
	Point center{ width / 2, height / 2 };
	unsigned short colourIndex = 0;
	setPixel(colours[colourIndex], i, j);
	Point curPos{ j, i };
	if (curPos.x < center.x + radius)
	{
		float distanceFromLeftMost = fabs(curPos.x - (center.x - radius));
		float factor = (distanceFromLeftMost / (radius));
		int distance = std::hypot(center.x - curPos.x, center.y - curPos.y);
		if (distance < radius && distance > radius*0.5*(factor)) {
			setPixel(colours[1], i, j);
		}
	}
}

//TODO: Return pretty colour combinations
vector<RGBTRIPLE> Flag::generateColours()
{
	// NOTE: this is B, G , R!!!
	vector<RGBTRIPLE> neutrals;
	neutrals.push_back(RGBTRIPLE{ 0, 0, 0 }); // black
	neutrals.push_back(RGBTRIPLE{ 255, 255, 255 }); // white
	neutrals.push_back(RGBTRIPLE{ 192, 192, 192 }); // silver

	vector<RGBTRIPLE> brightColours;
	brightColours.push_back(RGBTRIPLE{ 20, 20, 200 }); // red
	brightColours.push_back(RGBTRIPLE{ 30, 220, 30 }); // green
	brightColours.push_back(RGBTRIPLE{ 200, 30, 0 }); // blue
	brightColours.push_back(RGBTRIPLE{ 55, 175, 212 }); // gold

	vector<RGBTRIPLE> darkColours;
	darkColours.push_back(RGBTRIPLE{ 90, 30, 30 }); // dark blue
	darkColours.push_back(RGBTRIPLE{ 30, 100, 30 }); // dark green
	darkColours.push_back(RGBTRIPLE{ 20, 30, 100 }); // dark red
	vector<vector<RGBTRIPLE>> nonNeutralTypes;
	nonNeutralTypes.push_back(brightColours);
	nonNeutralTypes.push_back(darkColours);


	vector<RGBTRIPLE> colours;
	bool doubleNeutral = (*random)() % 2;
	if (doubleNeutral)
	{
		colours.push_back(neutrals[(*random)() % neutrals.size()]);
		vector<RGBTRIPLE> middleColour = nonNeutralTypes[(*random)() % nonNeutralTypes.size()];
		colours.push_back(middleColour[(*random)() % middleColour.size()]);
		colours.push_back(neutrals[(*random)() % neutrals.size()]);

	}
	else {
		vector<RGBTRIPLE> leftColours = nonNeutralTypes[(*random)() % nonNeutralTypes.size()];
		colours.push_back(leftColours[(*random)() % leftColours.size()]);
		colours.push_back(neutrals[(*random)() % neutrals.size()]);
		vector<RGBTRIPLE> rightColours = nonNeutralTypes[(*random)() % nonNeutralTypes.size()];
		colours.push_back(rightColours[(*random)() % rightColours.size()]);
	}
	return colours;
}

void Flag::setPixel(RGBTRIPLE colour, uint32_t x, uint32_t y)
{
	flag[(x*width + y) * 4] = colour.rgbtRed;
	flag[(x*width + y) * 4 + 1] = colour.rgbtGreen;
	flag[(x*width + y) * 4 + 2] = colour.rgbtBlue;
	flag[(x*width + y) * 4 + 3] = 0;
}

RGBTRIPLE Flag::getPixel(uint32_t x, uint32_t y)
{
	RGBTRIPLE colour;
	colour.rgbtRed = flag[(x*width + y) * 4];
	colour.rgbtGreen = flag[(x*width + y) * 4 + 1];
	colour.rgbtBlue = flag[(x*width + y) * 4 + 2];
	return colour;
}


RGBTRIPLE Flag::getPixel(uint32_t pos)
{
	RGBTRIPLE colour;
	colour.rgbtRed = flag[pos * 4];
	colour.rgbtGreen = flag[pos * 4 + 1];
	colour.rgbtBlue = flag[pos * 4 + 2];
	return colour;
}

unsigned char * Flag::getFlag()
{
	return flag;;
}
