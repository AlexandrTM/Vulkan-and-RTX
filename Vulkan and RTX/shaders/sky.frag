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
    
    // Gradient from horizon to zenith
    vec3 zenithColor = vec3(0.2, 0.5, 0.8); // Blueish color at the zenith
    vec3 horizonColor = vec3(0.8, 0.5, 0.2); // Orange color at the horizon
    vec3 baseColor = mix(horizonColor, zenithColor, t);
    
    // Atmospheric scattering
    float sunIntensity = max(dot(direction, sunDirection), 0.0);
    vec3 rayleigh = vec3(5.5, 13.0, 22.4); // Rayleigh scattering coefficients
    vec3 mie = vec3(21.0, 21.0, 21.0); // Mie scattering coefficients
    float sunElevation = max(0.0, dot(direction, vec3(0.0, 1.0, 0.0)));
    float atmosphereFactor = 1.0 - exp(-0.0001 * sunElevation * sunElevation); // Atmospheric scattering factor
    vec3 scattering = exp(-(rayleigh + mie) * atmosphereFactor);

    vec3 finalColor = baseColor * scattering + pow(sunIntensity, 128) * vec3(1.0, 0.8, 0.6);
    
    return finalColor;
}

void main() {
    vec3 direction = normalize(inPosition);
    vec3 sunDirection = normalize(ubo.sun);

    vec3 skyColor = getSkyColor(direction, sunDirection);
    
    outColor = vec4(skyColor, 1.0);
}