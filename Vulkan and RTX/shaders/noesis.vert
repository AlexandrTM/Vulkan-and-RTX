#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord0;
layout(location = 4) in vec2 inTexCoord1;
layout(location = 5) in ivec4 inBoneIDs;
layout(location = 6) in vec4 inBoneWeights;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec2 outTexCoord0; 

void main() 
{
	outTexCoord0 = inTexCoord0;
	outColor = inColor;
	gl_Position = vec4(inPosition, 1.0);
}