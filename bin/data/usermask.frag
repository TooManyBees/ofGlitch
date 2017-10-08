#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect usermask;
uniform float threshold;

in vec2 texCoordVarying;

out vec4 outputColor;

float thresholdColor(float edge, vec4 color) {
    return step(edge,
        // (color.r + color.g + color.b) / 3.0
        // max(color.r, max(color.g, color.b))
        color.r
    );
}

void main() {
    vec4 texelColor = texture(tex0, texCoordVarying);
    vec4 texelUser = texture(usermask, texCoordVarying);

    vec3 rgb;
    if (threshold > 0.0) {
        rgb = vec3(thresholdColor(threshold, texelColor));
    } else {
        rgb = texelColor.rgb;
    }

    outputColor = vec4(rgb, texelUser.r);
}
