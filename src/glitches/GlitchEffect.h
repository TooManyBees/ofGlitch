#pragma once
#include "ofMain.h"

class GlitchEffect {
public:
	virtual void update(ofImage &depthFrame, ofFloatColor color, float threshold) = 0;
	virtual void draw(ofRectangle space) = 0;
	virtual void drawBuffer(ofRectangle space) = 0;
};
