#include "RainbowNew.h"

void RainbowNew::init(int _width, int _height) {
	width = _width;
	height = _height;
	glitchFbo.allocate(width, height, GL_RGBA);
}

void RainbowNew::update(ofImage &depthFrame, ofImage &userFrame, ofFloatColor color, float _threshold, float expansion, glm::vec2 expansionVector) {
	reloadShaders();
	threshold = _threshold;

	glitchIn.begin();
	glitchIn.setUniform1f("threshold", threshold);
	glitchIn.setUniform1f("expansion", expansion);
	glitchIn.setUniform2f("expansionVector", expansionVector);
	glitchIn.setUniform3f("color", color.r, color.g, color.b);
	glitchIn.setUniformTexture("glitchTex", glitchFbo.getTexture(), 1);
	glitchIn.setUniformTexture("userTex", userFrame, 2);
	glitchIn.setUniform2f("resolution", width, height);

	glitchFbo.begin();

	ofDisableAlphaBlending();
	depthFrame.draw(0, 0, width, height);
	//depthFrame.drawSubsection(0, 0, width, height, 32, 24, width - 64, height - 48);
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

void RainbowNew::markShadersDirty() {
	shadersDirty = true;
}

void RainbowNew::reloadShaders() {
	if (shadersDirty) {
		ofLogNotice() << "Reloading shaders." << endl;
		shadersDirty = false;
		{
			glitchIn.load("identity.vert", "englitch2.frag");
			GLint err = glGetError();
			if (err != GL_NO_ERROR) {
				ofLogNotice() << "Shader 'englitch2.frag' failed to compile:" << endl << err << endl;
			}
		}
		{
			glitchOut.load("identity.vert", "beglitch2.frag");
			GLint err = glGetError();
			if (err != GL_NO_ERROR) {
				ofLogNotice() << "Shader 'beglitch2.frag' failed to compile:" << endl << err << endl;
			}
		}
	}
}
