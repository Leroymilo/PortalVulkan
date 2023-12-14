#include <string>
#include <unordered_set>

#include "../include/world.hpp"

World::World() {
	simple_brushes = {
		// floor 
		SimpleBrush(
			glm::vec3(-4.0f, -4.0f, -0.5f),
			glm::vec3(4.0f, 4.0f, 0.0f),
			"concrete_very_small_alt"
		),
		// wall x pos
		SimpleBrush(
			glm::vec3(4.0f, -4.0f, 0.0f),
			glm::vec3(4.5f, 4.0f, 4.0f),
			"concrete"
		),
		// wall x neg
		SimpleBrush(
			glm::vec3(-4.5f, -4.0f, 0.0f),
			glm::vec3(-4.0f, 4.0f, 4.0f),
			"concrete"
		),
		// wall y pos
		SimpleBrush(
			glm::vec3(-4.0f, 4.0f, 0.0f),
			glm::vec3(4.0f, 4.5f, 4.0f),
			"concrete_small"
		),
		// wall y pos
		SimpleBrush(
			glm::vec3(-4.0f, -4.5f, 0.0f),
			glm::vec3(4.0f, -4.0f, 4.0f),
			"concrete"
		),
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
			tex_names_set.insert(brush.tex_name);
		}
	}

	return tex_names;
}

void World::set_descriptor_sets(std::unordered_map<std::string, std::vector<int>> &set_indices_map) {
	for (SimpleBrush &brush : simple_brushes) {
		brush.set_descriptor_sets(set_indices_map[brush.tex_name]);
	}
}

void World::process_physics(GLFWwindow *window) {
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	player.process_physics(window, deltaTime);

	for (SimpleBrush &brush : simple_brushes) {
		if (player.collides(brush.get_collider_p())) {
			// printf("collision !\n");
		}
	}

	lastTime = currentTime;
}

void World::get_matrices(glm::mat4 *view) {
	*view = player.get_view_matrix();
}

void World::cmd_draw_indexed(RenderInfo &render_info) {
	for (SimpleBrush &brush :simple_brushes) {
		brush.cmd_draw_indexed(render_info);
	}
}