#version 460

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D bumpSampler;

layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec3 fragNorm;
layout(location = 3) in vec3 camPos;
layout(location = 4) in mat3 TBN;

layout(location = 0) out vec4 outColor;

const vec3 lightPos = vec3(1.3, 1.8, 2.3);

const float PI = 3.1415926535897932384626433;

void main() {
	// transform matrix to change bump to normal :
	const float theta = PI / 4;;
	const float alpha = - atan(1 / sqrt(2));

	const mat3 Y = mat3(
		cos(alpha), 0, -sin(alpha),
		0, 1, 0,
		sin(alpha), 0, cos(alpha)
	);

	const mat3 X = mat3(
		1, 0, 0,
		0, cos(theta), sin(theta),
		0, -sin(theta), cos(theta)
	);

	const mat3 T = Y * X;

	vec3 l = normalize(lightPos - fragPos);
	vec3 n = texture(bumpSampler, UV).rgb;
	n = TBN * T * vec3(n.x, n.y, n.z);

	float d = length(lightPos - fragPos);

	float diffuse_coef = clamp( dot(n, l), 0, 1 ) / (d * d);

	vec3 e = (camPos - fragPos).xyz;
	float specular_coef = clamp( dot(reflect(-l, n), e), 0, 1 );
	specular_coef = pow(specular_coef, 5) / (d * d);

	vec4 base_color = texture(texSampler, UV);

    outColor = base_color * (
		0.2 * length(n) + 		// ambient
		0.8 * diffuse_coef +	// diffuse
		0.5 * specular_coef		// specular
	);
}