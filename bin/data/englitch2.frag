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

float intify(float f) {
    return floor(f * 256) / 256;
}

float classicDiff(vec4 _glitch, vec4 _depth) {
    float glitch = intify(_glitch.a);
    float depth = intify(_depth.r);
    return abs(glitch - depth);
}

void main() {
    vec4 depthSample = texture(tex0, texCoordVarying);
    vec4 glitchSample = texture(glitchTex, texCoordVarying);
    float alpha = classicDiff(glitchSample, depthSample);
    vec3 mixed = step(threshold, alpha) * color + step(alpha, threshold) * glitchSample.rgb;
    outputColor = vec4(mixed, alpha);
}
