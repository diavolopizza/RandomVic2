#pragma once
#include <vector>
#include <map>
#include <Windows.h>
using namespace std;
class River
{

	int source;
	int currentEnd;
	River* endsIn;
	vector<River*> ingoing;
	map<int, River* > ingoingRivers;
public:
	vector<int> pixels;
	RGBTRIPLE colour;
	River();
	~River();
	void setSource(int	pixel);
	int getSource();
	int getCurrentEnd();
	void setcurrentEnd(int pixel);
	bool contains(int pixel);
	void addIngoing(River* ingoing, int pixel);
	vector<River*> getIngoing();
	void addEndsIn(River * river);
	River * getIngoingForKey(int pixel);

};

