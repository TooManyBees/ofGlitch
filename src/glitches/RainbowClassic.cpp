#include "RainbowClassic.h"

void RainbowClassic::init(int _width, int _height) {
	width = _width;
	height = _height;
	glitchIn.load("identity.vert", "englitch.frag");
	glitchOut.load("identity.vert", "beglitch.frag");
	glitchFbo.allocate(width, height, GL_RGBA);
}

void RainbowClassic::update(ofImage &depthFrame, ofFloatColor _color, float _threshold) {
	color = _color;
	threshold = _threshold;

	glitchIn.begin();
	glitchIn.setUniformTexture("glitchTex", glitchFbo.getTextureReference(), 1);
	glitchFbo.begin();
	depthFrame.draw(0, 0, width, height);
	glitchFbo.end();
	glitchIn.end();
}

void RainbowClassic::drawBuffer(ofRectangle space) {
	glitchFbo.draw(space);
}

void RainbowClassic::draw(ofRectangle space) {
	glitchOut.begin();
	glitchOut.setUniform1f("threshold", threshold);
	glitchOut.setUniform3f("color", color.r, color.g, color.b);
	glitchFbo.draw(space);
	glitchOut.end();
}
