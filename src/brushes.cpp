#include <stdio.h>
#include <vector>

#include "../include/brushes.hpp"

const float t = 1.f/3.f;
const float t2 = 2.f/3.f;

SimpleBrush::SimpleBrush(glm::vec3 min_point, glm::vec3 max_point) {
	this->min_point = min_point;
	this->max_point = max_point;

	// if (generate_buffers() != 0) {
	// 	printf("error while generating simple brush buffers!");
	// }

	// texture_id = loadDDS("uvtemplate.DDS");
}

bool SimpleBrush::generate_buffers(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
) {
	float min_x = min_point.x, min_y = min_point.y, min_z = min_point.z;
	float max_x = max_point.x, max_y = max_point.y, max_z = max_point.z;

	// the vertex index in indices where this brush starts
	vertex_offset = vertices->size();
	first_index = indices->size();

	// === filling vectors ===
	// face 1
	vertices->insert(vertices->end(), {
		{
			glm::vec3(min_x, max_y, max_z),
			glm::vec2(0, t),
			glm::vec3(0, 1, 0)
		}, {
			glm::vec3(max_x, max_y, max_z),
			glm::vec2(t, t),
			glm::vec3(0, 1, 0)
		}, {
			glm::vec3(max_x, max_y, min_z),
			glm::vec2(t, 0),
			glm::vec3(0, 1, 0)
		}, {
			glm::vec3(min_x, max_y, min_z),
			glm::vec2(0, 0),
			glm::vec3(0, 1, 0)
		}
	});

	// face 2
	vertices->insert(vertices->end(), {
		{
			glm::vec3(min_x, min_y, min_z),
			glm::vec2(t, t),
			glm::vec3(-1, 0, 0)
		}, {
			glm::vec3(min_x, min_y, max_z),
			glm::vec2(t2, t),
			glm::vec3(-1, 0, 0)
		}, {
			glm::vec3(min_x, max_y, max_z),
			glm::vec2(t2, 0),
			glm::vec3(-1, 0, 0)
		}, {
			glm::vec3(min_x, max_y, min_z),
			glm::vec2(t, 0),
			glm::vec3(-1, 0, 0)
		}
	});

	// face 3
	vertices->insert(vertices->end(), {
		{
			glm::vec3(min_x, min_y, max_z),
			glm::vec2(t2, t),
			glm::vec3(0, 0, 1)
		}, {
			glm::vec3(max_x, min_y, max_z),
			glm::vec2(1, t),
			glm::vec3(0, 0, 1)
		}, {
			glm::vec3(max_x, max_y, max_z),
			glm::vec2(1, 0),
			glm::vec3(0, 0, 1)
		}, {
			glm::vec3(min_x, max_y, max_z),
			glm::vec2(t2, 0),
			glm::vec3(0, 0, 1)
		}
	});

	// face 4
	vertices->insert(vertices->end(), {
		{
			glm::vec3(max_x, min_y, min_z),
			glm::vec2(0, t2),
			glm::vec3(0, 0, -1)
		}, {
			glm::vec3(min_x, min_y, min_z),
			glm::vec2(t, t2),
			glm::vec3(0, 0, -1)
		}, {
			glm::vec3(min_x, max_y, min_z),
			glm::vec2(t, t),
			glm::vec3(0, 0, -1)
		}, {
			glm::vec3(max_x, max_y, min_z),
			glm::vec2(0, t),
			glm::vec3(0, 0, -1)
		}
	});

	// face 5
	vertices->insert(vertices->end(), {
		{
			glm::vec3(max_x, min_y, max_z),
			glm::vec2(t, t2),
			glm::vec3(1, 0, 0)
		}, {
			glm::vec3(max_x, min_y, min_z),
			glm::vec2(t2, t2),
			glm::vec3(1, 0, 0)
		}, {
			glm::vec3(max_x, max_y, min_z),
			glm::vec2(t2, t),
			glm::vec3(1, 0, 0)
		}, {
			glm::vec3(max_x, max_y, max_z),
			glm::vec2(t, t),
			glm::vec3(1, 0, 0)
		}
	});

	// face 6
	vertices->insert(vertices->end(), {
		{
			glm::vec3(min_x, min_y, min_z),
			glm::vec2(t2, t2),
			glm::vec3(0, -1, 0)
		}, {
			glm::vec3(max_x, min_y, min_z),
			glm::vec2(1, t2),
			glm::vec3(0, -1, 0)
		}, {
			glm::vec3(max_x, min_y, max_z),
			glm::vec2(1, t),
			glm::vec3(0, -1, 0)
		}, {
			glm::vec3(min_x, min_y, max_z),
			glm::vec2(t2, t),
			glm::vec3(0,- 1, 0)
		}
	});

	// filling indices, given that faces were well defined
	for (unsigned int i = 0; i < 24; i += 4) {
		indices->insert(indices->end(), {
			i+0u, i+1u, i+2u, i+2u, i+3u, i+0u
		});
	}

	return true;
}

void SimpleBrush::cmd_draw_indexed(VkCommandBuffer &commandBuffer) {
	vkCmdDrawIndexed(
		commandBuffer, 36, 1,
		first_index, vertex_offset, 0
	);
}