#pragma once
#include "ofMain.h"
#include "FreeImage.h"

class GifSaver {
public:
	GifSaver(int w, int h, float f, string path) : width(w), height(h), fps(f), recordingPath(path) {
		frames = new vector<ofPixels*>;
		frames->reserve(60);
	};
	~GifSaver() {
		if (!detached) {
			for (auto frame : *frames) {
				delete frame;
			}
			delete frames;
		}
	}
	void push(ofPixels* pixels);
	void save();
private:
	float fps = 30.f;
	string recordingPath;
	unsigned int width;
	unsigned int height;
	vector<ofPixels*>* frames;
	bool detached = false;
};
