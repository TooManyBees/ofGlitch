#pragma once
#include "GlitchEffect.h"

class RainbowNew : public GlitchEffect {
public:
  void init(int width, int height) override;
  void update(ofImage &depthFrame, ofFloatColor color, float threshold, float expansion = 1.0, glm::vec2 expansionVector = glm::vec2(0.0, 0.0)) override;
  void draw(ofRectangle space) override;
  void drawBuffer(ofRectangle space) override;
  void markShadersDirty() override;
private:
  ofFbo glitchFbo;
  ofShader glitchIn;
  ofShader glitchOut;
  int width;
  int height;
  float threshold;
  bool shadersDirty = true;
  void reloadShaders();
};
