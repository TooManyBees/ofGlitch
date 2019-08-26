﻿#pragma once
#include "ofMain.h"
#include "GlitchEffect.h"

class RainbowClassic : public GlitchEffect {
public:
	void init(int width, int height);
	void update(ofImage &depthFrame, ofFloatColor color, float threshold) override;
	void draw(ofRectangle space) override;
	void drawBuffer(ofRectangle space) override;
private:
	ofFbo glitchFbo;
	ofShader glitchIn;
	ofShader glitchOut;
	ofFloatColor color;
	int width;
	int height;
	float threshold;
};
