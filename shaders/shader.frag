#version 460

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D normalSampler;

layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec3 camPos;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 outColor;

const vec3 light_pos = vec3(-2, -3, -4);

void main() {
	vec3 l = normalize(light_pos - fragPos.xyz);
	vec3 n = TBN * normalize(texture(normalSampler, UV).rgb * 2.0 - 1.0);

	float diffuse_coef = clamp( dot(n, l), 0, 1 );

	vec3 e = (camPos - fragPos).xyz;
	float specular_coef = clamp( dot(reflect(l, n), e), 0, 1 );
	specular_coef = pow(specular_coef, 5);

	vec4 base_color = texture(texSampler, UV);

    outColor = base_color * (
		0.2 + 					// ambient
		0.5 * diffuse_coef //+	// diffuse
		//0.3 * specular_coef		// specular
	);
}