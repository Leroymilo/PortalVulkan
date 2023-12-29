#version 460

layout(binding = 0) uniform UniformBufferObject {
    // mat4 model;
    mat4 view;
    mat4 proj;
} ubo;
layout(binding = 1) uniform UboInstance {
	mat4 model;
} uboInstance;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inTangent;
layout(location = 2) in vec3 inBitangent;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec2 inTexCoord;

layout(location = 0) out vec2 UV;
layout(location = 1) out vec3 frag_pos;
layout(location = 2) out vec3 frag_norm;
layout(location = 3) out vec3 cam_pos;
layout(location = 4) out mat3 TBN;

void main() {
    gl_Position = ubo.proj * ubo.view * uboInstance.model * vec4(inPosition, 1.0);

    UV = inTexCoord;
	
	frag_pos = (uboInstance.model * vec4(inPosition, 1)).xyz;

	frag_norm = inNormal.xyz;
	
	cam_pos = (inverse(ubo.view) * vec4(0, 0, 0, 1)).xyz;

	TBN = mat3(uboInstance.model) * mat3(
		inTangent,
		inBitangent,
		inNormal
	);
}