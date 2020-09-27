#include "Flag.h"

Flag::Flag(ranlux24* random)
{
	this->random = random;

	// load the template
	unsigned char* targaimage;
	targaimage = (unsigned char*)tga_load("C:\\Users\\Paul\\Documents\\Visual Studio 2017\\Projects\\\RandomVic2\\RandomVic2\\resources\\gfx\\flags\\template.tga", &width, &height, TGA_TRUECOLOR_32);
	this->flag = targaimage;

	int type = (*random)() % 3;


	colours = generateColours();
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
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
				flagType = PLAIN;
				plain(i, j);
				break;
			}
			default:
				break;
			}
		}
	}
	type = 2;//;(*random)() % 2;
	colours = generateColours();
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			switch (type)
			{
			case 0:
			{
				symbolType = MOON;
				halfMoon(i, j);
				break;
			}
			case 1:
			{
				symbolType = STAR;
				star(i, j, 0.5, 0.5, 0.35);
				break;
			}
			case 2:
			{
				symbolType = MOONSTAR;
				halfMoonStars(i, j);
				break;
			}
			case 3:
			{
				symbolType = SQUARE;
				squareSquared(i, j);
				break;
			}
			case 4:
			{
				symbolType = CIRCLE;
				circle(i, j);
				break;
			}
			case 5:
			{
				symbolType = MULTISTAR;
				circle(i, j);
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

void Flag::plain(int i, int j)
{
	setPixel(colours[0], i, j);
}

void Flag::squareSquared(int i, int j)
{
	if (abs((int)j - width / 2) < 16 && abs((int)i - height / 2) < 16)
		setPixel(colours[1], i, j);
}

void Flag::circle(int i, int j)
{
	struct Point { int x; int y; };
	Point center{ width / 2, height / 2 };
	Point curPos{ j, i };
	auto distance = abs(std::hypot(center.x - curPos.x, center.y - curPos.y));
	if (distance < 15) {
		setPixel(colours[1], i, j);
	}
}

void Flag::halfMoon(int i, int j)
{
	auto radius = 15.0;
	struct Point { int x; int y; };
	Point center{ width / 2, height / 2 };
	Point curPos{ j, i };
	if (curPos.x < center.x + radius)
	{
		float distanceFromLeftMost = fabs(curPos.x - (center.x - radius));
		float factor = (distanceFromLeftMost / (radius));
		auto distance = std::hypot(center.x - curPos.x, center.y - curPos.y);
		if (distance < radius && distance > radius*0.5*(factor)) {
			setPixel(colours[3], i, j);
		}
	}
}
bg::model::point<double, 2, bg::cs::cartesian> rotate(float angle, bg::model::point<double, 2, bg::cs::cartesian> point, bg::model::point<double, 2, bg::cs::cartesian> pivot)
{
	//float angle = 1.2566370614;
	double s = sin(angle); // angle is in radians
	double c = cos(angle); // angle is in radians

	double xnew = (bg::get<0>(point) - bg::get<0>(pivot)) * c + (bg::get<1>(point) - bg::get<1>(pivot)) * s;
	double ynew = -(bg::get<0>(point) - bg::get<0>(pivot)) * s + (bg::get<1>(point) - bg::get<1>(pivot)) * c;
	bg::model::point<double, 2, bg::cs::cartesian> point2(xnew + bg::get<0>(pivot), ynew + bg::get<1>(pivot));
	return point2;

}
void Flag::star(int i, int j, float xPos, float yPos, float size)
{
	double angle = 72 * 3.14 / 180;
	typedef boost::geometry::model::d2::point_xy<double> point_type;
	typedef boost::geometry::model::linestring<point_type> linestring_type;


	bg::model::point<double, 2, bg::cs::cartesian> center(width*xPos, height*yPos);
	bg::model::point<double, 2, bg::cs::cartesian> curPos(j, i);
	vector<bg::model::point<double, 2, bg::cs::cartesian>> points;
	bg::model::point<double, 2, bg::cs::cartesian> one(bg::get<0>(center), bg::get<1>(center) + size * width); // up
	points.push_back(one);
	for (int i = 0; i < 4; i++)
	{
		points.push_back(rotate(angle, points[i], center));
	}
	vector<linestring_type> lines;
	for (auto point : points)
	{
		linestring_type line;
		line.push_back(point_type(bg::get<0>(center), bg::get<1>(center)));
		line.push_back(point_type(bg::get<0>(point), bg::get<1>(point)));
		lines.push_back(line);

	}
	for (int index = 0; index < points.size(); index++)
	{
		double lineDistance = fabs(bg::distance(curPos, lines[index]));
		double yDistance = bg::distance(curPos, points[index]);

		double centerDistance = bg::distance(curPos, center);
		if (yDistance < (size*width)) {

			double factor = yDistance / (size*(float)width);
			if (lineDistance < (width / 4 * size)*factor)
				setPixel(colours[3], i, j);

		}
	}
}

void Flag::halfMoonStars(int i, int j)
{
	halfMoon(i, j);
	star(i, j, 0.65, 0.7, 0.07);
	star(i, j, 0.7, 0.5, 0.07);
	star(i, j, 0.65, 0.3, 0.07);
}

//TODO: Return pretty colour combinations
vector<RGBTRIPLE> Flag::generateColours()
{
	// NOTE: this is B, G , R!!!
	vector<RGBTRIPLE> neutrals;
	neutrals.push_back(RGBTRIPLE{ 0, 0, 0 }); // black
	neutrals.push_back(RGBTRIPLE{ 255, 255, 255 }); // white
	//neutrals.push_back(RGBTRIPLE{ 192, 192, 192 }); // silver

	vector<RGBTRIPLE> brightColours;
	brightColours.push_back(RGBTRIPLE{ 20, 20, 200 }); // red
	brightColours.push_back(RGBTRIPLE{ 34,139,34 }); // green
	brightColours.push_back(RGBTRIPLE{ 200, 30, 0 }); // blue
	brightColours.push_back(RGBTRIPLE{ 0, 215, 255 }); // gold

	vector<RGBTRIPLE> darkColours;
	darkColours.push_back(RGBTRIPLE{ 90, 30, 30 }); // dark blue
	darkColours.push_back(RGBTRIPLE{ 0, 100, 0 }); // dark green
	darkColours.push_back(RGBTRIPLE{ 20, 30, 100 }); // dark red
	darkColours.push_back(RGBTRIPLE{ 45, 82, 160 }); // gold-ish red

	vector<vector<RGBTRIPLE>> nonNeutralTypes;
	nonNeutralTypes.push_back(brightColours);
	nonNeutralTypes.push_back(darkColours);


	vector<RGBTRIPLE> colours;
	bool doubleNeutral = (*random)() % 2;
	//if (doubleNeutral)
	//{
	//	colours.push_back(neutrals[(*random)() % neutrals.size()]);
	//	vector<RGBTRIPLE> middleColour = nonNeutralTypes[(*random)() % nonNeutralTypes.size()];
	//	colours.push_back(middleColour[(*random)() % middleColour.size()]);
	//	colours.push_back(neutrals[(*random)() % neutrals.size()]);
	//}
	{
		vector<RGBTRIPLE> leftColours = nonNeutralTypes[(*random)() % nonNeutralTypes.size()];
		colours.push_back(leftColours[(*random)() % leftColours.size()]);
		colours.push_back(neutrals[(*random)() % neutrals.size()]);
		vector<RGBTRIPLE> rightColours = nonNeutralTypes[(*random)() % nonNeutralTypes.size()];
		colours.push_back(rightColours[(*random)() % rightColours.size()]);

		colours.push_back(brightColours[(*random)() % brightColours.size()]);
		//while(colours[3] == colours)
	}
	return colours;
}

void Flag::setPixel(RGBTRIPLE colour, uint32_t x, uint32_t y)
{
	flag[(x*width + y) * 4] = colour.rgbtRed;
	flag[(x*width + y) * 4 + 1] = colour.rgbtGreen;
	flag[(x*width + y) * 4 + 2] = colour.rgbtBlue;
	flag[(x*width + y) * 4 + 3] = 255;
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
