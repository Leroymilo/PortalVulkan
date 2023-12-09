#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragPos;
layout(location = 2) out vec4 fragNorm;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;
	fragPos = ubo.model * vec4(inPosition, 1);
	fragNorm = ubo.model * vec4(inNormal, 0);
}