#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect glitchTex;
uniform sampler2DRect userTex;
uniform float expansion;
uniform vec2 expansionVector;
uniform vec2 resolution;

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

vec2 expand(vec2 pos, float amount, vec2 dir) {
    vec2 direction = dir + vec2(0.5);
    vec2 ratio = vec2(resolution.x / resolution.y, 1);
    vec2 centered = (pos/resolution - direction) * ratio;
    return ((centered * amount / ratio) + direction) * resolution;
}

const float BOOST_USER = 0.5;

vec4 highlightUsers(vec2 pos) {
    float user = texture(userTex, pos).r;
    vec3 depth = texture(tex0, pos).rgb;
    vec3 bump = step(0.5, user) * (vec3(1.0) - depth) * BOOST_USER;
    return vec4(depth + bump, 1.0);
}

void main() {
    vec2 pos = expand(texCoordVarying, expansion, expansionVector);
    // vec4 depthSample = texture(tex0, texCoordVarying);
    vec4 depthSample = highlightUsers(texCoordVarying);
    vec4 glitchSample = texture(glitchTex, texCoordVarying);
    outputColor = mysteryDiff3(glitchSample, depthSample);
}
