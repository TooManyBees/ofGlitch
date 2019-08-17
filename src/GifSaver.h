#pragma once
#include "ofMain.h"
#include "FreeImage.h"

class GifSaver {
public:
	GifSaver(int w, int h, string path) : width(w), height(h), recordingPath(path) {
		frames = new vector<ofPixels*>;
		frames->reserve(60);
	};
	~GifSaver() {
		if (!detached) {
			delete frames;
		}
	}
	void push(ofPixels* pixels);
	void save();
private:
	string recordingPath;
	unsigned int width;
	unsigned int height;
	vector<ofPixels*>* frames;
	bool detached = false;
};