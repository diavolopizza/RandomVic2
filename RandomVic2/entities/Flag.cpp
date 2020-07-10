#include "Flag.h"


Flag::Flag(ranlux48* random)
{
	this->random = random;

	int type = (*random)() % 4;
	switch (type)
	{
	case 0:
	{
		tricolore();
		break;
	}
	case 1:
	{
		rotatedTricolore();
		break;
	}
	case 2:
	{
		squareSquared();
		break;
	}
	case 3:
	{
		circle();
		break;
	}
	default:
		break;
	}
}




Flag::~Flag()
{
}

void Flag::tricolore()
{
	unsigned char* targaimage;
	targaimage = (unsigned char*)tga_load("C:\\Users\\Paul\\Documents\\Visual Studio 2017\\Projects\\\RandomVic2\\RandomVic2\\resources\\gfx\\flags\\template.tga", &width, &height, TGA_TRUECOLOR_32);
	this->flag = targaimage;
	if (flag == NULL)
	{
		printf("Failed to read image!\n");
		printf(tga_error_string(tga_get_last_error()));
	}

	vector<RGBTRIPLE> colours = generateColours();

	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			unsigned short colourIndex = j / (width / 3);
			setPixel(colours[colourIndex], i, j);
		}
	}
}

void Flag::rotatedTricolore()
{
	unsigned char* targaimage;
	targaimage = (unsigned char*)tga_load("C:\\Users\\Paul\\Documents\\Visual Studio 2017\\Projects\\\RandomVic2\\RandomVic2\\resources\\gfx\\flags\\template.tga", &width, &height, TGA_TRUECOLOR_32);
	this->flag = targaimage;
	if (flag == NULL)
	{
		printf("Failed to read image!\n");
		printf(tga_error_string(tga_get_last_error()));
	}

	vector<RGBTRIPLE> colours = generateColours();

	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			unsigned short colourIndex = i / (height / 3);
			setPixel(colours[colourIndex], i, j);
		}
	}
}

void Flag::squareSquared()
{
	unsigned char* targaimage;
	targaimage = (unsigned char*)tga_load("C:\\Users\\Paul\\Documents\\Visual Studio 2017\\Projects\\\RandomVic2\\RandomVic2\\resources\\gfx\\flags\\template.tga", &width, &height, TGA_TRUECOLOR_32);
	this->flag = targaimage;
	if (flag == NULL)
	{
		printf("Failed to read image!\n");
		printf(tga_error_string(tga_get_last_error()));
	}

	vector<RGBTRIPLE> colours = generateColours();

	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			unsigned short colourIndex = 0;
			setPixel(colours[colourIndex], i, j);
			if (abs((int)j - width / 2) < 16 && abs((int)i - height / 2) < 16)
				setPixel(colours[1], i, j);
		}
	}
}

void Flag::circle()
{
	unsigned char* targaimage;
	targaimage = (unsigned char*)tga_load("C:\\Users\\Paul\\Documents\\Visual Studio 2017\\Projects\\\RandomVic2\\RandomVic2\\resources\\gfx\\flags\\template.tga", &width, &height, TGA_TRUECOLOR_32);
	this->flag = targaimage;
	if (flag == NULL)
	{
		printf("Failed to read image!\n");
		printf(tga_error_string(tga_get_last_error()));
	}

	vector<RGBTRIPLE> colours = generateColours();
	struct Point { int x; int y; };
	Point center{ width / 2, height / 2 };
	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			unsigned short colourIndex = 0;
			setPixel(colours[colourIndex], i, j);
			Point curPos{ j, i };
			int distance = abs(std::hypot(center.x - curPos.x, center.y - curPos.y));
			if (distance < 15) {
				setPixel(colours[1], i, j);
			}
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