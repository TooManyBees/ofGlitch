#pragma once

#include "ofMain.h"
#include "openni.h"
#include "nite.h"

# define MAX_DEPTH 10000

class OniManager {
public:
	//~OniManager();
	bool setup(int w, int h, int fps, bool mirror = true);

	void getColorFrame(ofImage* image);
	void getDepthFrame(ofImage* image);
	void getUserMask(bool *users);
	void getUserFrame(ofImage* image);

private:
	openni::Device device;
	openni::VideoStream depthStream;
	openni::VideoStream colorStream;
	openni::VideoFrameRef colorFrame;

	//void updateColorFrame();
	//uint64_t colorFrameTimestamp;

	nite::UserTracker userTracker;
	nite::UserTrackerFrameRef userFrame;
	//void updateUserFrame();
	//uint64_t userFrameTimestamp;

	unsigned char* pDepthMap;
	float depthHistogram[MAX_DEPTH];
	void histogram(float* pHistogram, openni::VideoFrameRef& frame);
};