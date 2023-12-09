#include <stdio.h>
#include <string>
#include <vector>

#include "../include/brushes.hpp"

SimpleBrush::SimpleBrush(glm::vec3 min_point, glm::vec3 max_point, std::string tex_name):
	min_point(min_point), max_point(max_point), tex_name(tex_name) {}

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
			glm::vec2(1, 1),git 
			glm::vec3(0, 1, 0)
		}, {
			glm::vec3(max_x, max_y, max_z),
			glm::vec2(1, 0),
			glm::vec3(0, 1, 0)
		}, {
			glm::vec3(max_x, max_y, min_z),
			glm::vec2(0, 0),
			glm::vec3(0, 1, 0)
		}, {
			glm::vec3(min_x, max_y, min_z),
			glm::vec2(0, 1),
			glm::vec3(0, 1, 0)
		}
	});

	// face 2
	vertices->insert(vertices->end(), {
		{
			glm::vec3(min_x, min_y, min_z),
			glm::vec2(1, 1),
			glm::vec3(-1, 0, 0)
		}, {
			glm::vec3(min_x, min_y, max_z),
			glm::vec2(1, 0),
			glm::vec3(-1, 0, 0)
		}, {
			glm::vec3(min_x, max_y, max_z),
			glm::vec2(0, 0),
			glm::vec3(-1, 0, 0)
		}, {
			glm::vec3(min_x, max_y, min_z),
			glm::vec2(0, 1),
			glm::vec3(-1, 0, 0)
		}
	});

	// face 3
	vertices->insert(vertices->end(), {
		{
			glm::vec3(min_x, min_y, max_z),
			glm::vec2(1, 1),
			glm::vec3(0, 0, 1)
		}, {
			glm::vec3(max_x, min_y, max_z),
			glm::vec2(1, 0),
			glm::vec3(0, 0, 1)
		}, {
			glm::vec3(max_x, max_y, max_z),
			glm::vec2(0, 0),
			glm::vec3(0, 0, 1)
		}, {
			glm::vec3(min_x, max_y, max_z),
			glm::vec2(0, 1),
			glm::vec3(0, 0, 1)
		}
	});

	// face 4
	vertices->insert(vertices->end(), {
		{
			glm::vec3(max_x, min_y, min_z),
			glm::vec2(1, 1),
			glm::vec3(0, 0, -1)
		}, {
			glm::vec3(min_x, min_y, min_z),
			glm::vec2(1, 0),
			glm::vec3(0, 0, -1)
		}, {
			glm::vec3(min_x, max_y, min_z),
			glm::vec2(0, 0),
			glm::vec3(0, 0, -1)
		}, {
			glm::vec3(max_x, max_y, min_z),
			glm::vec2(0, 1),
			glm::vec3(0, 0, -1)
		}
	});

	// face 5
	vertices->insert(vertices->end(), {
		{
			glm::vec3(max_x, min_y, max_z),
			glm::vec2(1, 1),
			glm::vec3(1, 0, 0)
		}, {
			glm::vec3(max_x, min_y, min_z),
			glm::vec2(1, 0),
			glm::vec3(1, 0, 0)
		}, {
			glm::vec3(max_x, max_y, min_z),
			glm::vec2(0, 0),
			glm::vec3(1, 0, 0)
		}, {
			glm::vec3(max_x, max_y, max_z),
			glm::vec2(0, 1),
			glm::vec3(1, 0, 0)
		}
	});

	// face 6
	vertices->insert(vertices->end(), {
		{
			glm::vec3(min_x, min_y, min_z),
			glm::vec2(1, 1),
			glm::vec3(0, -1, 0)
		}, {
			glm::vec3(max_x, min_y, min_z),
			glm::vec2(1, 0),
			glm::vec3(0, -1, 0)
		}, {
			glm::vec3(max_x, min_y, max_z),
			glm::vec2(0, 0),
			glm::vec3(0, -1, 0)
		}, {
			glm::vec3(min_x, min_y, max_z),
			glm::vec2(0, 1),
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

void SimpleBrush::set_descriptor_sets(std::vector<int> &set_indices) {
	this->descriptor_set_indices = set_indices;
}

void SimpleBrush::cmd_draw_indexed(RenderInfo &render_info) {
	int set_index = descriptor_set_indices[render_info.current_frame];
	VkDescriptorSet descriptor_set = render_info.descriptor_sets[set_index];

	vkCmdBindDescriptorSets(
		render_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		render_info.pipeline_layout, 0, 1, &descriptor_set,
		0, nullptr
	);

	vkCmdDrawIndexed(
		render_info.command_buffer, 36, 1,
		first_index, vertex_offset, 0
	);
}