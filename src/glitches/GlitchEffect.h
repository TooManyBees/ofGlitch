#pragma once
#include "ofMain.h"

class GlitchEffect {
public:
	virtual void init(int _width, int _height) = 0;
	virtual void update(ofImage &depthFrame, ofFloatColor color, float threshold, float expansion = 1.0, glm::vec2 expansionVector = glm::vec2(0.0, 0.0)) = 0;
	virtual void draw(ofRectangle space) = 0;
	virtual void drawBuffer(ofRectangle space) = 0;
	virtual void markShadersDirty() = 0;
};
