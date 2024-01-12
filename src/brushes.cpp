#include <stdio.h>
#include <string>
#include <vector>

#include "../include/brushes.hpp"

#define assertm(exp, msg) assert(((void)msg, exp))


// Brush


void Brush::set_descriptor_sets(std::vector<int> &set_indices) {
	this->descriptor_set_indices = set_indices;
}

void Brush::cmd_draw_indexed(RenderInfo &render_info) {
	int set_index = descriptor_set_indices[render_info.current_frame];
	VkDescriptorSet descriptor_set = render_info.descriptor_sets[set_index];

	uint32_t dynamic_offset = 0;

	vkCmdBindDescriptorSets(
		render_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		render_info.pipeline_layout, 0, 1, &descriptor_set,
		1, &dynamic_offset
	);

	vkCmdDrawIndexed(
		render_info.command_buffer, 36, 1,
		first_index, vertex_offset, 0
	);
}


// SimpleBrush

SimpleBrush::SimpleBrush(const glm::vec3 &min_point, const glm::vec3 &max_point, std::string tex_name):
	min_point(min_point), max_point(max_point),
	collider(min_point, max_point)
{
	assertm(min_point.x < max_point.x, "Invalid points for SimpleBrush : not ordered on x axis");
	assertm(min_point.y < max_point.y, "Invalid points for SimpleBrush : not ordered on y axis");
	assertm(min_point.z < max_point.z, "Invalid points for SimpleBrush : not ordered on z axis");
	this->tex_name = tex_name;
}

Collision::AABox *SimpleBrush::get_collider_p() {
	return &collider;
}

bool SimpleBrush::generate_buffers(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
) {
	// the vertex index in indices where this brush starts
	vertex_offset = vertices->size();
	first_index = indices->size();

	float min_x = min_point.x, min_y = min_point.y, min_z = min_point.z;
	float max_x = max_point.x, max_y = max_point.y, max_z = max_point.z;
	float dx = max_x - min_x, dy = max_y - min_y, dz = max_z - min_z;
	dx /= 2, dy /= 2, dz /= 2;	// each texture is 2m x 2m

	// === filling vectors ===
	// face 1
	vertices->insert(vertices->end(), {
		{
			glm::vec3(min_x, min_y, max_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),
			glm::vec2(0, dy)
		}, {
			glm::vec3(max_x, min_y, max_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),
			glm::vec2(dx, dy)
		}, {
			glm::vec3(max_x, max_y, max_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),
			glm::vec2(dx, 0)
		}, {
			glm::vec3(min_x, max_y, max_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),
			glm::vec2(0, 0)
		}
	});

	// face 2
	vertices->insert(vertices->end(), {
		{
			glm::vec3(min_x, max_y, min_z),
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(-1, 0, 0),
			glm::vec2(0, dz)
		}, {
			glm::vec3(min_x, min_y, min_z),
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(-1, 0, 0),
			glm::vec2(dy, dz)
		}, {
			glm::vec3(min_x, min_y, max_z),
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(-1, 0, 0),
			glm::vec2(dy, 0)
		}, {
			glm::vec3(min_x, max_y, max_z),
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(-1, 0, 0),
			glm::vec2(0, 0)
		}
	});

	// face 3
	vertices->insert(vertices->end(), {
		{
			glm::vec3(min_x, min_y, min_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(0, -1, 0),
			glm::vec2(0, dz)
		}, {
			glm::vec3(max_x, min_y, min_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(0, -1, 0),
			glm::vec2(dx, dz)
		}, {
			glm::vec3(max_x, min_y, max_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(0, -1, 0),
			glm::vec2(dx, 0)
		}, {
			glm::vec3(min_x, min_y, max_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(0, -1, 0),
			glm::vec2(0, 0)
		}
	});

	// face 4
	vertices->insert(vertices->end(), {
		{
			glm::vec3(max_x, max_y, min_z),
			glm::vec3(-1, 0, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 1, 0),
			glm::vec2(0, dz)
		}, {
			glm::vec3(min_x, max_y, min_z),
			glm::vec3(-1, 0, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 1, 0),
			glm::vec2(dx, dz)
		}, {
			glm::vec3(min_x, max_y, max_z),
			glm::vec3(-1, 0, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 1, 0),
			glm::vec2(dx, 0)
		}, {
			glm::vec3(max_x, max_y, max_z),
			glm::vec3(-1, 0, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 1, 0),
			glm::vec2(0, 0)
		}
	});

	// face 5
	vertices->insert(vertices->end(), {
		{
			glm::vec3(max_x, min_y, min_z),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec2(0, dz)
		}, {
			glm::vec3(max_x, max_y, min_z),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec2(dy, dz)
		}, {
			glm::vec3(max_x, max_y, max_z),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec2(dy, 0)
		}, {
			glm::vec3(max_x, min_y, max_z),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec2(0, 0)
		}
	});

	// face 6
	vertices->insert(vertices->end(), {
		{
			glm::vec3(min_x, max_y, min_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, -1),
			glm::vec2(0, dy)
		}, {
			glm::vec3(max_x, max_y, min_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, -1),
			glm::vec2(dx, dy)
		}, {
			glm::vec3(max_x, min_y, min_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, -1),
			glm::vec2(dx, 0)
		}, {
			glm::vec3(min_x, min_y, min_z),
			glm::vec3(1, 0, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, -1),
			glm::vec2(0, 0)
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


// PrismBrush

PrismBrush::PrismBrush(
	const glm::vec3 &min_point, const glm::vec3 &max_point,
	const std::vector<glm::vec3> &base, std::string tex_name
):
	min_point(min_point), max_point(max_point),
	base(base), collider(min_point, max_point, base)
{
	int base_d = -1;
	for (int i = 0; i < 3; i++) {
		if (min_point[i] != 0 || max_point[i] != 0) {
			assertm(base_d == -1, "Invalid min or max point for PrismBrush : points should be on same axis");
			base_d = i;
		}
	}

	assertm(
		min_point[base_d] != max_point[base_d],
		"Invalid points for PrismBrush : point should be distincts"
	);

	glm::vec3 height = max_point - min_point;

	for (const glm::vec3 &point: base) {
		assertm(
			glm::dot(height, point) == 0,
			"Invalid base point for PrismBrush : not orthogonal to height"
		);
	}

	// TODO : check if polygon is convex and ordered
}

Collision::AAPrism *PrismBrush::get_collider_p() {
	return &collider;
}

bool PrismBrush::generate_buffers(
	std::vector<Vertex> *vertices,
	std::vector<uint32_t> *indices
) {
	// the vertex index in indices where this brush starts
	vertex_offset = vertices->size();
	first_index = indices->size();

	// TODO mdr
	// issue : how to choose UVs?

	return false;
}