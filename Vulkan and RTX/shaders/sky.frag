#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 sun;
	vec3 observer;
} ubo;

float fade(float t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float grad(int hash, float x, float y, float z) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float perlinNoise(float x, float y, float z) {
    int p[512];
    int permutation[] = {132, 54, 153, 222, 114, 20, 164, 248, 157, 255, 172, 97, 219, 51, 153, 141, 176, 14, 155, 184, 230, 197, 212, 118, 139, 205, 143, 117, 37, 156, 82, 147, 217, 225, 87, 18, 209, 58, 149, 139, 126, 185, 19, 93, 110, 117, 98, 189, 81, 68, 145, 99, 250, 100, 165, 20, 17, 86, 242, 62, 125, 227, 250, 34, 146, 57, 183, 110, 46, 10, 104, 179, 103, 169, 212, 200, 135, 51, 199, 89, 186, 229, 129, 174, 237, 105, 197, 64, 30, 158, 178, 215, 200, 7, 238, 125, 174, 135, 106, 132, 136, 192, 159, 196, 194, 248, 136, 223, 248, 56, 35, 79, 157, 30, 139, 129, 221, 127, 28, 113, 162, 191, 164, 112, 240, 104, 212, 244, 175, 109, 187, 192, 165, 247, 182, 103, 247, 36, 223, 193, 199, 153, 36, 15, 180, 109, 179, 227, 194, 184, 154, 23, 47, 186, 77, 150, 14, 184, 62, 252, 87, 152, 39, 219, 90, 67, 155, 172, 98, 139, 11, 55, 203, 94, 131, 110, 198, 223, 176, 119, 121, 198, 250, 228, 22, 142, 143, 109, 154, 59, 16, 74, 202, 234, 29, 1, 121, 223, 148, 29, 172, 208, 82, 102, 241, 85, 188, 177, 193, 175, 27, 108, 9, 229, 119, 188, 174, 30, 35, 100, 184, 152, 201, 98, 46, 136, 26, 32, 55, 130, 37, 156, 29, 100, 65, 22, 153, 69, 111, 197, 50, 88, 56, 233, 230, 196, 168, 110, 77, 120, 37, 91, 215, 128, 57, 86};

    for (int i = 0; i < 256; i++) {
        p[256 + i] = p[i] = permutation[i];
    }
    
    // Find unit grid cell containing point
    int X = int(floor(x)) & 255;
    int Y = int(floor(y)) & 255;
    int Z = int(floor(z)) & 255;

    // Find relative x, y in grid cell
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    // Compute fade curves for x, y
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    // Hash coordinates of the 4 square corners
    int a = (p[ X      & 255] + Y) & 255;
    int b = (p[(X + 1) & 255] + Y) & 255;

    int aa = p[p[a    ] + Z];
    int ab = p[p[a + 1] + Z];
    int ba = p[p[b    ] + Z];
    int bb = p[p[b + 1] + Z];

    float p0 = grad(aa, x    , y    , z);
    float p1 = grad(ba, x - 1, y    , z);
    float p2 = grad(ab, x    , y - 1, z);
    float p3 = grad(bb, x - 1, y - 1, z);
    float p4 = grad(aa, x    , y    , z - 1);
    float p5 = grad(ba, x - 1, y    , z - 1);
    float p6 = grad(ab, x    , y - 1, z - 1);
    float p7 = grad(bb, x - 1, y - 1, z - 1);

    float q0 = lerp(p0, p1, u);
    float q1 = lerp(p2, p3, u);
    float q2 = lerp(p4, p5, u);
    float q3 = lerp(p6, p7, u);

    float r0 = lerp(q0, q1, v);
    float r1 = lerp(q2, q3, v);

    // And add blended results from 4 corners of the square
    float res = lerp(r0, r1, w);
    // return res; // result is in range [-1, 1]
    return (res + 1) / 2; // result is in range [0, 1]
}

vec3 getSunColor(vec3 direction, vec3 sunDirection) {
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

    vec3 sunColor = baseColor * scattering + pow(sunIntensity, 128) * vec3(1.0, 0.8, 0.6);
    
    return sunColor;
}

vec3 getCloudColor(vec3 direction, vec3 sunDirection) {
    float cloudDensity = perlinNoise(direction.x * 0.5, direction.y * 0.5, direction.z * 0.5);
    cloudDensity = smoothstep(0.0, 1.0, cloudDensity); // Threshold to create distinct clouds

    vec3 cloudColor = mix(vec3(0.2), vec3(0.75), cloudDensity); // Light grey clouds
    float sunLight = max(dot(direction, sunDirection), 0.0);
    cloudColor += sunLight * vec3(0.2); // Sunlight effect on clouds

    return cloudColor;
}

void main() {
    vec3 direction = normalize(inPosition);
    vec3 sunDirection = normalize(ubo.sun);

    vec3 sunColor = getSunColor(direction, sunDirection);
    //vec3 cloudColor = getCloudColor(direction, sunDirection);

    //vec3 finalColor = (sunColor + cloudColor) / 2;

    outColor = vec4(sunColor, 1.0);
}