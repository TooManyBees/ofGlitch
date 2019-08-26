#include "RainbowNew.h"

void RainbowNew::init(int _width, int _height) {
	width = _width;
	height = _height;
	glitchIn.load("identity.vert", "englitch2.frag");
	glitchOut.load("identity.vert", "beglitch2.frag");
	glitchFbo.allocate(width, height, GL_RGBA);
}

void RainbowNew::update(ofImage &depthFrame, ofFloatColor color, float _threshold) {
	threshold = _threshold;

	glitchIn.begin();
	glitchIn.setUniform1f("threshold", threshold);
	glitchIn.setUniform3f("color", color.r, color.g, color.b);
	glitchIn.setUniformTexture("glitchTex", glitchFbo.getTextureReference(), 1);

	glitchFbo.begin();

	ofDisableAlphaBlending();
	depthFrame.draw(0, 0, width, height);
	ofEnableAlphaBlending();
	glitchFbo.end();

	glitchIn.end();
}

void RainbowNew::draw(ofRectangle space) {
	glitchOut.begin();
	glitchOut.setUniform1f("threshold", threshold);
	glitchFbo.draw(space);
	glitchOut.end();
}

void RainbowNew::drawBuffer(ofRectangle space) {
	glitchFbo.draw(space);
}