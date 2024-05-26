#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 sun;
	vec3 observer;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord0;

layout(location = 0) out vec4 outColor;

void main() {
    float visibilityRange = 150.0;

    float distanceToFragment = length(ubo.observer - inPosition);

    outColor = texture(texSampler, inTexCoord0) * vec4(inColor, 1.0);

    if (distanceToFragment > visibilityRange) {
        outColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
}