#version 450

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outPosition;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 sun;
	vec3 observer;
    float visibilityRange;
} ubo;

void main() {
    gl_Position = ubo.proj * ubo.model * vec4(inPosition, 1.0);
    outPosition = inPosition;
}