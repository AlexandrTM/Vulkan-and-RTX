#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 sun;
	vec3 observer;
	float visibilityRange;
} ubo;

layout(set = 0, binding = 1) readonly buffer BoneUniformBufferObject {
    mat4 boneTransforms[256];
} boneUBO;

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

void main() {		
	mat4 boneTransform = mat4(0.0);
	vec4 transformedPosition = vec4(0.0);

    for (int i = 0; i < 1; i++) {
        if (inBoneWeights[i] > 0.0) {
            boneTransform += inBoneWeights[i] * boneUBO.boneTransforms[inBoneIDs[i]];
			//transformedPosition += vec4(inBoneWeights[i]);
        }
    }
	transformedPosition = boneTransform * vec4(inPosition, 1.0);
	if (transformedPosition == vec4(0.0)) {
        transformedPosition = vec4(inPosition, 1.0);
    }

	// for (int i = 0; i < 256; i++) {
	// 	if (boneUBO.boneTransforms[i] != mat4(0.0f)) {
	// 		transformedPosition = vec4(inPosition, 1.0);
	// 	}
	// }

	// if ((boneUBO.boneTransforms[inBoneIDs[0]] + 
	// 	 boneUBO.boneTransforms[inBoneIDs[1]] + 
	// 	 boneUBO.boneTransforms[inBoneIDs[2]] + 
	// 	 boneUBO.boneTransforms[inBoneIDs[3]]) == mat4(0.0)) {
	// 		transformedPosition = vec4(inPosition, 0.0);
	// }
	// else {
	// 	transformedPosition = vec4(inPosition, 1.0);
	// }

	vec3 normalSpace = normalize(mat3(ubo.model) * normalize(inNormal));
	
	vec3 toSun = normalize(ubo.sun/* - inPosition*/);
	vec3 toObserver = normalize(ubo.observer - vec3(transformedPosition));
	vec3 reflected = reflect(-toSun, inNormal);
	//vec3 halfWay = normalize(toSun + toObserver);
		
	float directionalLight = min(max(dot(normalSpace, toSun), 0) + 0.05, 1.0);
	
	float specular = 0.0;
	
	if (dot(inNormal, toSun) > 0)
	{
		specular = pow(max(dot(inNormal, reflected), 0.0), 5.0);
	}
		
	outPosition = vec3(ubo.model * transformedPosition);
	outColor = inColor * (specular + directionalLight);
    outTexCoord0 = inTexCoord0;
    gl_Position = ubo.proj * ubo.view * ubo.model * transformedPosition;
}