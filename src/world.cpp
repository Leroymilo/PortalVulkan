#include <stdio.h>

#include "../include/world.hpp"

World::World() {
	simple_brushes = {
		SimpleBrush(glm::vec3(-2.0f, -2.0f, -0.5f), glm::vec3(2.0f, 2.0f, 0.0f)),
		SimpleBrush(glm::vec3(2.0f, -2.0f, 0.0f), glm::vec3(2.5f, 2.0f, 4.0f))
	};
}

bool World::get_geometry(
	std::vector<Vertex> *vertices,
	std::vector<uint32_t> *indices
) {
	for (SimpleBrush &brush :simple_brushes) {
		if (!brush.generate_buffers(vertices, indices)) {
			printf("error while generating simple brush buffers!");
		}
	}

	return true;
}

void World::get_matrices(GLFWwindow *window, glm::mat4 *proj, glm::mat4 *view) {
	camera.get_matrices(window, proj, view);
}

void World::cmd_draw_indexed(VkCommandBuffer &commandBuffer) {
	for (SimpleBrush &brush :simple_brushes) {
		brush.cmd_draw_indexed(commandBuffer);
	}
}