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
    float gamma = 1.25;
    float visibilityRange = 200.0;
    //float contrast = 0.1;

    float distanceToFragment = distance(ubo.observer, inPosition);

    vec4 texColor =  texture(texSampler, inTexCoord0);
    texColor.rgb = pow(texColor.rgb, vec3(1.0 / gamma));
    //texColor.rgb = (texColor.rgb - 0.5) * contrast + 0.5;

    float fogFactor = exp(-distanceToFragment * 0.00015);

    outColor = mix(vec4(1.0, 1.0, 1.0, 1.0), texColor * vec4(inColor, 1.0), fogFactor);  

    if (distanceToFragment > visibilityRange) {
        discard;
        //outColor = vec4(0.0, ubo.observer.x/10, 0.0, 1.0);
    }
}