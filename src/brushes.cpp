#include <stdio.h>
#include <vector>

#include "../include/brushes.hpp"

const float t = 1.f/3.f;
const float t2 = 2.f/3.f;

SimpleBrush::SimpleBrush(glm::vec3 min_point, glm::vec3 max_point) {
	this->min_point = min_point;
	this->max_point = max_point;

	if (generate_buffers() != 0) {
		printf("error while generating simple brush buffers!");
	}

	// texture_id = loadDDS("uvtemplate.DDS");
}

int SimpleBrush::generate_buffers() {
	float min_x = min_point.x, min_y = min_point.y, min_z = min_point.z;
	float max_x = max_point.x, max_y = max_point.y, max_z = max_point.z;

	std::vector < glm::vec3 > vertices;
	std::vector < glm::vec2 > uvs;
	std::vector < glm::vec3 > normals;
	std::vector < unsigned int > indices;

	unsigned int i = 0u;

	// === filling vectors ===
	// face 1
	vertices.insert(vertices.end(), {
		glm::vec3(min_x, max_y, max_z),
		glm::vec3(max_x, max_y, max_z),
		glm::vec3(max_x, max_y, min_z),
		glm::vec3(min_x, max_y, min_z)
	});
	uvs.insert(uvs.end(), {
		glm::vec2(0, t),
		glm::vec2(t, t),
		glm::vec2(t, 0),
		glm::vec2(0, 0)
	});
	normals.insert(normals.end(), 4, glm::vec3(0, 1, 0));

	// face 2
	vertices.insert(vertices.end(), {
		glm::vec3(min_x, min_y, min_z),
		glm::vec3(min_x, min_y, max_z),
		glm::vec3(min_x, max_y, max_z),
		glm::vec3(min_x, max_y, min_z)
	});
	uvs.insert(uvs.end(), {
		glm::vec2(t, t),
		glm::vec2(t2, t),
		glm::vec2(t2, 0),
		glm::vec2(t, 0)
	});
	normals.insert(normals.end(), 4, glm::vec3(-1, 0, 0));

	// face 3
	vertices.insert(vertices.end(), {
		glm::vec3(min_x, min_y, max_z),
		glm::vec3(max_x, min_y, max_z),
		glm::vec3(max_x, max_y, max_z),
		glm::vec3(min_x, max_y, max_z)
	});
	uvs.insert(uvs.end(), {
		glm::vec2(t2, t),
		glm::vec2(1, t),
		glm::vec2(1, 0),
		glm::vec2(t2, 0)
	});
	normals.insert(normals.end(), 4, glm::vec3(0, 0, 1));

	// face 4
	vertices.insert(vertices.end(), {
		glm::vec3(max_x, min_y, min_z),
		glm::vec3(min_x, min_y, min_z),
		glm::vec3(min_x, max_y, min_z),
		glm::vec3(max_x, max_y, min_z)
	});
	uvs.insert(uvs.end(), {
		glm::vec2(0, t2),
		glm::vec2(t, t2),
		glm::vec2(t, t),
		glm::vec2(0, t)
	});
	normals.insert(normals.end(), 4, glm::vec3(0, 0, -1));

	// face 5
	vertices.insert(vertices.end(), {
		glm::vec3(max_x, min_y, max_z),
		glm::vec3(max_x, min_y, min_z),
		glm::vec3(max_x, max_y, min_z),
		glm::vec3(max_x, max_y, max_z)
	});
	uvs.insert(uvs.end(), {
		glm::vec2(t, t2),
		glm::vec2(t2, t2),
		glm::vec2(t2, t),
		glm::vec2(t, t)
	});
	normals.insert(normals.end(), 4, glm::vec3(1, 0, 0));

	// face 6
	vertices.insert(vertices.end(), {
		glm::vec3(min_x, min_y, min_z),
		glm::vec3(max_x, min_y, min_z),
		glm::vec3(max_x, min_y, max_z),
		glm::vec3(min_x, min_y, max_z)
	});
	uvs.insert(uvs.end(), {
		glm::vec2(t2, t2),
		glm::vec2(1, t2),
		glm::vec2(1, t),
		glm::vec2(t2, t)
	});
	normals.insert(normals.end(), 4, glm::vec3(0, -1, 0));

	// filling indices, given that faces were well defined
	for (unsigned int i = 0; i < 24; i += 4) {
		indices.insert(indices.end(), {
			i+0u, i+1u, i+2u, i+0u, i+2u, i+3u
		});
	}

	return 0;
}