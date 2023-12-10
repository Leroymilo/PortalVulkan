#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inTangent;
layout(location = 2) in vec3 inBitangent;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec2 inTexCoord;

layout(location = 0) out vec2 UV;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec3 fragNorm;
layout(location = 3) out vec3 camPos;
layout(location = 4) out mat3 TBN;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

    UV = inTexCoord;
	
	fragPos = (ubo.model * vec4(inPosition, 1)).xyz;

	fragNorm = inNormal.xyz;
	
	camPos = (inverse(ubo.view) * vec4(0, 0, 0, 1)).xyz;

	TBN = mat3(ubo.model) * mat3(
		inTangent,
		inBitangent,
		inNormal
	);
}