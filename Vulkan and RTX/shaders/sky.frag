#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 sun;
	vec3 viewer;
} ubo;

vec3 getSkyColor(vec3 direction, vec3 sunDirection) {
    float t = direction.y * 0.5 + 0.5; // Map y from [-1, 1] to [0, 1]
    
    // Gradient from horizon to sky
    vec3 horizonColor = vec3(0.8, 0.5, 0.2);
    vec3 skyColor = vec3(0.2, 0.5, 0.8);
    vec3 baseColor = mix(horizonColor, skyColor, t);
    
    // Sun effect
    float sunIntensity = max(dot(direction, sunDirection), 0.0);
    vec3 sunColor = vec3(1.0, 0.8, 0.6);
    vec3 finalColor = mix(baseColor, sunColor, pow(sunIntensity, 32.0));
    
    return finalColor;
}

void main() {
    vec3 direction = normalize(inPosition);
    vec3 sunDirection = normalize(ubo.sun);

    vec3 skyColor = getSkyColor(direction, sunDirection);
    
    outColor = vec4(skyColor, 1.0);
}