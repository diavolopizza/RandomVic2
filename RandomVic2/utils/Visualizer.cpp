#include "Visualizer.h"



Visualizer::Visualizer(Terrain& terrainGenerator)
{
	this->terrainGenerator = terrainGenerator;
}


Visualizer::~Visualizer()
{
}


//void Visualizer::displayImage(Bitmap bitmap)
//{
	//Mat image(bitmap.bInfoHeader.biHeight, bitmap.bInfoHeader.biWidth, 16);
	//if (bitmap.bInfoHeader.biBitCount == 24) {
	//	image.data = bitmap.getBuffer();
	//}
	//else {
	//	image = imread("C:/Users/Paul/Documents/Visual Studio 2017/RandomVic2/debugMap/terrain.bmp");   // Read the file
	//	for (uint32_t i = 0; i < bitmap.bInfoHeader.biSizeImage; i++)
	//	{
	//		image.data[1078 + i] = bitmap.getValueAtIndex(i);
	//	}
	//}
	//uint32_t xRes = 1920;
	//uint32_t yRes = 1080;
	//double xScaleFactor = (double)xRes / (double)bitmap.bInfoHeader.biWidth;
	//double yScaleFactor = (double)yRes / (double)bitmap.bInfoHeader.biHeight;
	////if (yScaleFactor < xScaleFactor)
	////	//cv::resize(image, image, Size(bitmap.bInfoHeader.biWidth * yScaleFactor, bitmap.bInfoHeader.biHeight * yScaleFactor));
	////else {
	////	//resize(image, image, Size(bitmap.bInfoHeader.biWidth * xScaleFactor, bitmap.bInfoHeader.biHeight * xScaleFactor));
	////}
	//if (!image.data)                              // Check for invalid input
	//{
	//	cout << "Could not open or find the image" << std::endl;
	//	return;
	//}
	////imshow("Display window", image);
	//waitKey(1);/*
	//HWND hwnd = (HWND)cvGetWindowHandle("Display window");
	//BringWindowToTop(hwnd);
	//ShowWindow(hwnd, SW_RESTORE);*/
                           // Wait for a keystroke in the window
//}

//void Visualizer::initializeWindow()
//{
	//namedWindow("Display window");// Create a window for display.
	//moveWindow("Display window", 0, 0);
	//resizeWindow("Display window", 1920, 1080);
	//cvSetWindowProperty("Display window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
//}

void Visualizer::prettyRivers(Bitmap* riverBMP, const Bitmap heightMap)
{
	const uint32_t maxRiverColour = 10;
	for (uint32_t i = 0; i < (uint32_t)(riverBMP->bInfoHeader.biWidth * riverBMP->bInfoHeader.biHeight); i++)
	{
		if (heightMap.getValueAtIndex(i) > Data::getInstance().seaLevel)
			riverBMP->setValueAtIndex(i, 255);
		else
			riverBMP->setValueAtIndex(i, 254);
	}
	for (River* river : terrainGenerator.rivers) {
		if (river->pixels.size() < 10)
			continue;
		uint32_t riverColour = 2;
		for (uint32_t pix : river->pixels) {
			if (riverColour < maxRiverColour && river->getIngoingForKey(pix) != nullptr) {
				riverColour += (uint32_t)river->getIngoingForKey(pix)->getIngoing().size() + 1u;
				if (riverColour > maxRiverColour)
					riverColour = maxRiverColour;
			}
			riverBMP->setValueAtIndex(pix, riverColour);
		}
	}
	for (River* river : terrainGenerator.rivers) {
		riverBMP->setValueAtIndex(river->getSource(), 0);
	}
}