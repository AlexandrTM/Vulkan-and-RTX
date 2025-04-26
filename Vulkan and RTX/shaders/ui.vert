#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord0;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outTexCoord0; 

void main() 
{
	outTexCoord0 = inTexCoord0;
	gl_Position = vec4(inPosition, 1.0);
}