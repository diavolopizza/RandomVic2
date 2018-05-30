#pragma once
#include <vector>
#include  <Windows.h>
using namespace std;
class River
{

	uint32_t source;
public:
	vector<uint32_t> pixels;
	vector<River*> ingoing;
	vector<River*> outgoing;
	RGBTRIPLE colour;
	River();
	~River();
	void defineColour();
	void setSource(uint32_t	pixel);
};

