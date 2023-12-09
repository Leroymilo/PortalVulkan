#version 460

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragPos;
layout(location = 2) in vec4 fragNorm;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
}