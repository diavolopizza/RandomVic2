#pragma once
#include <vector>
#include  <Windows.h>
using namespace std;
class River
{

	uint32_t source;
	uint32_t currentEnd;
public:
	vector<uint32_t> pixels;
	vector<River*> ingoing;
	vector<River*> outgoing;
	RGBTRIPLE colour;
	River();
	~River();
	void defineColour();
	void setSource(uint32_t	pixel);
	uint32_t getSource();
	uint32_t getCurrentEnd();
	void setcurrentEnd(uint32_t pixel);
	bool contains(uint32_t pixel);
};

