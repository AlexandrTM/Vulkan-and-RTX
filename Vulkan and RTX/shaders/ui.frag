#version 450

layout(binding = 2) uniform sampler2D diffuseSampler;
layout(binding = 3) uniform sampler2D emissiveSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord0;

layout(location = 0) out vec4 outColor;

void main() 
{
	//outColor = vec4(1.0, 0.0, 0.0, 0.5);
	outColor = texture(diffuseSampler, inTexCoord0);
}