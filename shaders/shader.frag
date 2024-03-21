#version 460

layout(binding = 2) uniform sampler2D tex_sampler;
layout(binding = 3) uniform sampler2D bump_sampler;

layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 frag_pos;
layout(location = 2) in vec3 frag_norm;
layout(location = 3) in vec3 cam_pos;
layout(location = 4) in mat3 TBN;

layout(location = 0) out vec4 outColor;

const vec3 lightPos = vec3(-2, 2, 4.1);
const float lightPow = 40;

const float PI = 3.1415926535897932384626433;

void main() {
	// transform matrix to change bump to normal :
	const float theta = PI / 4;;
	const float alpha = - atan(1 / sqrt(2));

	const mat3 T = mat3(
		cos(alpha), 0, -sin(alpha),
		0, 1, 0,
		sin(alpha), 0, cos(alpha)
	) * mat3(
		1, 0, 0,
		0, cos(theta), sin(theta),
		0, -sin(theta), cos(theta)
	);

	vec3 l = normalize(lightPos - frag_pos);
	vec3 n = texture(bump_sampler, UV).rgb;
	n = TBN * T * vec3(n.x, n.z, n.y);

	float ambient_coef = (1 + length(n)) / 2;
	n = normalize(n);

	float d = length(lightPos - frag_pos);

	float diffuse_coef = clamp( dot(n, l), 0, 1 ) * lightPow / (d * d);

	vec3 e = normalize(cam_pos - frag_pos);
	float specular_coef = clamp( dot(e, reflect(-l, n)), 0, 1 );
	specular_coef = pow(specular_coef, 5) * lightPow / (d * d);

	// avoids lighting on backsides from normal map
	if (dot(frag_norm, l) <= 0) {
		diffuse_coef = 0;
		specular_coef = 0;
	}

	vec4 base_color = texture(tex_sampler, UV);

    outColor = base_color * (
		0.2 * ambient_coef + 	// ambient
		0.8 * diffuse_coef +	// diffuse
		0.5 * specular_coef		// specular
	);
}