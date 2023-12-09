#include <string>
#include <unordered_map>

#include <stdio.h>

#include "../include/world.hpp"

World::World() {
	simple_brushes = {
		SimpleBrush(
			glm::vec3(-2.0f, -2.0f, -0.5f),
			glm::vec3(2.0f, 2.0f, 0.0f),
			"concrete_very_small_alt"
		),
		SimpleBrush(
			glm::vec3(2.0f, -2.0f, 0.0f),
			glm::vec3(2.5f, 2.0f, 4.0f),
			"concrete"
		)
	};
}

void World::get_geometry(
	std::vector<Vertex> *vertices,
	std::vector<uint32_t> *indices
) {
	for (SimpleBrush &brush :simple_brushes) {
		if (!brush.generate_buffers(vertices, indices)) {
			printf("error while generating simple brush buffers!");
		}
	}
}

std::vector<std::string> World::get_tex_names() {
	std::vector<std::string> tex_names;
	std::unordered_set<std::string> tex_names_set;

	for (SimpleBrush &brush : simple_brushes) {
		if (tex_names_set.count(brush.tex_name) == 0) {
			tex_names.push_back(brush.tex_name);
		}
	}

	return tex_names;
}

void World::set_descriptor_sets(std::unordered_map<std::string, std::vector<int>> &set_indices_map) {
	for (SimpleBrush &brush : simple_brushes) {
		brush.set_descriptor_sets(set_indices_map[brush.tex_name]);
	}
}

void World::get_matrices(GLFWwindow *window, glm::mat4 *proj, glm::mat4 *view) {
	camera.get_matrices(window, proj, view);
}

void World::cmd_draw_indexed(RenderInfo &render_info) {
	for (SimpleBrush &brush :simple_brushes) {
		brush.cmd_draw_indexed(render_info);
	}
}