#version 150

uniform sampler2DRect tex0;
uniform float threshold;
uniform vec3 color;

in vec2 texCoordVarying;

out vec4 outputColor;

float thresholdColor(float edge, vec4 color) {
    return step(edge,
        // (color.r + color.g + color.b) / 3.0
        max(color.r, max(color.g, color.b))
    );
}

void main() {
    vec4 glitchBuffer = texture(tex0, texCoordVarying);

    float alpha = thresholdColor(threshold, glitchBuffer);

    outputColor = vec4(color, alpha);
}
