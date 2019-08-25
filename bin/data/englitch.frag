#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect glitchTex;

in vec2 texCoordVarying;

out vec4 outputColor;

vec3 mysteryDiff(vec4 glitchSample, vec4 depthSample) {
  vec3 diff = abs(glitchSample - depthSample).rgb;
  return diff - (step(depthSample.rgb / 2.0, diff) * vec3(0.005, 0.005, 0.005));
}

void main() {
    vec4 depthSample = texture(tex0, texCoordVarying);
    vec4 glitchSample = texture(glitchTex, texCoordVarying);
    vec3 diffed = mysteryDiff(glitchSample, depthSample);
    outputColor = vec4(diffed, 1.0);
}
