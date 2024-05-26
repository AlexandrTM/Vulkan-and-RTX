#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 sun;
	vec3 observer;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord0;
layout(location = 4) in vec2 inTexCoord1;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec2 outTexCoord0; 

void main() {		
	vec3 normalSpace = normalize(mat3(ubo.model) * normalize(inNormal));
	
	vec3 toSun = normalize(ubo.sun/* - inPosition*/);
	vec3 toObserver = normalize(ubo.observer - inPosition);
	vec3 reflected = reflect(-toSun, inNormal);
	//vec3 halfWay = normalize(toSun + toObserver);
		
	float directionalLight = min(max(dot(normalSpace, toSun), 0) + 0.05, 1.0);
	
	float specular = 0.0;
	
	if (dot(inNormal, toSun) > 0)
	{
		specular = pow(max(dot(inNormal, reflected), 0.0), 5.0);
	}
		
	outPosition = vec3(ubo.model * vec4(inPosition, 1.0));
	outColor = inColor * (specular + directionalLight);
    outTexCoord0 = inTexCoord0;
    gl_Position =  ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
}