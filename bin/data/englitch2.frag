#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect glitchTex;
uniform vec3 color;
uniform float threshold;

in vec2 texCoordVarying;

out vec4 outputColor;

float mysteryDiff(vec4 glitchSample, vec4 depthSample) {
  float diff = abs(glitchSample.a - depthSample.r);
  return diff - (step(depthSample.r / 2.0, diff) * 0.005);
}

void main() {
    vec4 depthSample = texture(tex0, texCoordVarying);
    vec4 glitchSample = texture(glitchTex, texCoordVarying);
    float alpha = mysteryDiff(glitchSample, depthSample);
    vec3 mixed = step(threshold, alpha) * color + step(alpha, threshold) * glitchSample.rgb;
    outputColor = vec4(color, alpha);
}
