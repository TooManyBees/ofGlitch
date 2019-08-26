#version 150

uniform sampler2DRect tex0;
uniform float threshold;

in vec2 texCoordVarying;

out vec4 outputColor;

void main() {
    vec4 glitchBuffer = texture(tex0, texCoordVarying);
    outputColor = vec4(glitchBuffer.rgb, step(threshold, glitchBuffer.a));
}
