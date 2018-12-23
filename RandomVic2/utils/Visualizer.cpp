#include "Visualizer.h"



Visualizer::Visualizer()
{
}


Visualizer::~Visualizer()
{
}


void Visualizer::displayImage(Bitmap * bitmap)
{
	Mat image(bitmap->bitmapinfoheader.biHeight, bitmap->bitmapinfoheader.biWidth, 16);
	if (bitmap->bitmapinfoheader.biBitCount == 24) {
		image.data = bitmap->getBuffer();
	}
	else {
		image = imread("C:/Users/Paul/Documents/Visual Studio 2017/RandomVic2/debugMap/terrain.bmp");   // Read the file
		for (int i = 0; i < bitmap->bitmapinfoheader.biSizeImage; i++)
		{
			image.data[1078 + i] = bitmap->getValueAt(i);
		}
	}
	uint32_t xRes = 1920;
	uint32_t yRes = 1080;
	double xScaleFactor = (double)xRes / (double)bitmap->bitmapinfoheader.biWidth;
	double yScaleFactor = (double)yRes / (double)bitmap->bitmapinfoheader.biHeight;
	if (yScaleFactor < xScaleFactor)
		resize(image, image, Size(bitmap->bitmapinfoheader.biWidth * yScaleFactor, bitmap->bitmapinfoheader.biHeight * yScaleFactor));
	else {
		resize(image, image, Size(bitmap->bitmapinfoheader.biWidth * xScaleFactor, bitmap->bitmapinfoheader.biHeight * xScaleFactor));
	}
	if (!image.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return;
	}
	imshow("Display window", image);
	waitKey(1);/*
	HWND hwnd = (HWND)cvGetWindowHandle("Display window");
	BringWindowToTop(hwnd);
	ShowWindow(hwnd, SW_RESTORE);*/
                           // Wait for a keystroke in the window
}

void Visualizer::initializeWindow()
{
	namedWindow("Display window", CV_WINDOW_AUTOSIZE);// Create a window for display.
	moveWindow("Display window", 0, 0);
	//resizeWindow("Display window", 1920, 1080);
	//cvSetWindowProperty("Display window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
}
