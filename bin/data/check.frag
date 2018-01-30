#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect usermask;
uniform vec2 resolution;
uniform float outside;
uniform float size;
uniform float time;
uniform float threshold;

in vec2 texCoordVarying;

out vec4 outputColor;

// from shadertoy
// vec2 uv = floor(S * p.xy * vec2(iResolution.x / iResolution.y, 1) / iResolution.xy);
// c = vec4(vec3(mod(uv.x + uv.y, 2.)), 1);
float checker(vec2 uv, float size) {
    vec2 xy = floor(size * uv * vec2(resolution.x / resolution.y, 1) / resolution.xy);
    return mod(xy.x + xy.y, 2.0);
}

void main() {
    vec2 middle = resolution * 0.5;

    float cosFactor = cos(time);
    float sinFactor = sin(time);
	mat2 rotation = mat2(cosFactor, -sinFactor, sinFactor, cosFactor);
    vec2 texCoordRotated = (texCoordVarying - middle) * rotation;

    //vec4 texelGlitch = texture(tex0, texCoordVarying);
    vec4 texelUser = texture(usermask, texCoordVarying);

    float checks = checker(texCoordRotated, size);

	float user = texelUser.r;

    float mask = abs(user - outside);

    outputColor = vec4(vec3(checks), mask);
}
