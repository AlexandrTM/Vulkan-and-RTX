#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord0;

layout(location = 0) out vec4 outColor;

void main() 
{
	outColor = vec4(inColor, 1.0);
}