#pragma once
#include <vector>
#include <map>
#include <Windows.h>
using namespace std;
class River
{

	uint32_t source;
	uint32_t currentEnd;
	River* endsIn;
	vector<River*> ingoing;
	map<uint32_t, River* > ingoingRivers;
public:
	vector<uint32_t> pixels;
	RGBTRIPLE colour;
	River();
	~River();
	void setSource(uint32_t	pixel);
	uint32_t getSource();
	uint32_t getCurrentEnd();
	void setcurrentEnd(uint32_t pixel);
	bool contains(uint32_t pixel);
	void addIngoing(River* ingoing, uint32_t pixel);
	vector<River*> getIngoing();
	void addEndsIn(River * river);
	River * getIngoingForKey(uint32_t pixel);

};

