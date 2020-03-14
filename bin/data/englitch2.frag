#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect glitchTex;
uniform vec3 color;
uniform float threshold;
uniform float expansion;
uniform vec2 expansionVector;
uniform vec2 resolution;

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

vec2 expand(vec2 pos, float amount, vec2 dir) {
    vec2 direction = dir + vec2(0.5);
    vec2 ratio = vec2(resolution.x / resolution.y, 1);
    vec2 centered = (pos/resolution - direction) * ratio;
    return ((centered * amount / ratio) + direction) * resolution;
}

void main() {
    vec2 pos = expand(texCoordVarying, expansion, expansionVector);
    vec4 depthSample = texture(tex0, pos);
    vec4 glitchSample = texture(glitchTex, pos);
    float alpha = classicDiff(glitchSample, depthSample);
    vec3 mixed = mix(color, glitchSample.rgb, step(threshold, alpha));
    outputColor = vec4(mixed, alpha);
}
