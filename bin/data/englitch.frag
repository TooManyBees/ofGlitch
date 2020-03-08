#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect glitchTex;

in vec2 texCoordVarying;

out vec4 outputColor;

vec4 mysteryDiff(vec4 glitchSample, vec4 depthSample) {
  vec3 diff = abs(glitchSample - depthSample).rgb;
  vec3 d2 = diff - (step(depthSample.rgb / 2.0, diff) * vec3(0.005, 0.005, 0.005));
  return vec4(d2, 1.0);
}

const float tick = 1.0 / 255.0;

vec4 mysteryDiff2(vec4 a, vec4 b) {
    vec3 c = abs(a.rgb - b.rgb);
    return vec4(c, b.a - tick);
}

vec4 intify(vec4 f) {
    return floor(f * 256) / 256;
}

vec4 mysteryDiff3(vec4 _a, vec4 _b) {
    vec4 a = intify(_a);
    vec4 b = intify(_b);
    vec3 c = abs(a.rgb - b.rgb);
    return vec4(c, 1);
}

void main() {
    vec4 depthSample = texture(tex0, texCoordVarying);
    vec4 glitchSample = texture(glitchTex, texCoordVarying);
    outputColor = mysteryDiff3(glitchSample, depthSample);
}
