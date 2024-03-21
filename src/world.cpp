#include <string>
#include <unordered_set>

#include <iostream>

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

	models.push_back(
		Model(
			"metal_box", "metal_box"
		)
	);

	cubes.push_back(
		Cube(&(models[0]), 0.68)
	);
	cubes[0].set_rot_pos(
		glm::vec3(0, 0, 4),
		(float)M_PI/4.f * glm::normalize(glm::vec3(1, 1, 0))
	);

	cubes.push_back(
		Cube(&(models[0]), 0.68)
	);
	cubes[1].set_rot_pos(
		glm::vec3(0, 0, 7),
		(float)0 * glm::normalize(glm::vec3(0, 1, 1))
	);
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

	for (Model &model: models) {
		if (!model.generate_buffers(vertices, indices)) {
			printf("error while generating cubes buffers!");
		}
	}
}

size_t World::count_dyna_ubo() {
	return 1 + cubes.size();
	// the first model matrix is identity
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

	for (Model &model: models) {
		if (tex_names_set.count(model.tex_name) == 0) {
			tex_names.push_back(model.tex_name);
			tex_names_set.insert(model.tex_name);
		}
	}

	return tex_names;
}

void World::set_descriptor_sets(std::unordered_map<std::string, std::vector<int>> &set_indices_map) {
	for (SimpleBrush &brush : simple_brushes) {
		brush.set_descriptor_sets(set_indices_map[brush.tex_name]);
	}

	for (Model &model: models) {
		model.set_descriptor_sets(set_indices_map[model.tex_name]);
	}
}

void World::process_physics(GLFWwindow *window) {
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	deltaTime = std::min(deltaTime, 1.f/60.f);

	player.process_physics(window, deltaTime);
	
	for (Cube &cube: cubes) {
		cube.process_physics(deltaTime);
	}

	Collision::Sphere *player_collider = player.get_collider_p();

	for (SimpleBrush &brush : simple_brushes) {
		glm::vec4 dir_dist = Collision::resolve(player_collider, brush.get_collider_p());
		if (dir_dist.w > 0) {
			player.nudge(dir_dist);
		}
	}

	for (int i = 0; i < cubes.size(); i++) {

		Cube &cube = cubes[i];
		Collision::Cube *cube_collider = cube.get_collider_p();

		for (SimpleBrush &brush : simple_brushes) {
			glm::vec4 dir_dist = Collision::resolve(cube_collider, brush.get_collider_p());
			
			if (dir_dist.w > 0) {
				cube.resolve(dir_dist, deltaTime);
			}
		}

		for (int j = 0; j < cubes.size(); j++) {
			if (i == j) continue;

			glm::vec4 dir_dist = Collision::resolve(cube_collider, cubes[j].get_collider_p());
			if (dir_dist.w > 0) {
				dir_dist.w /= 2;
				cube.resolve(dir_dist, deltaTime);
			}
		}
	}

	lastTime = currentTime;
}



void World::get_view(glm::mat4 *view) {
	*view = player.get_view_matrix();
}

void World::set_model_matrix(
	glm::mat4 *matrix_array,
	uint32_t alignment, uint32_t index,
	const glm::mat4 &new_matrix
) {
	glm::mat4* model_mat = (glm::mat4*)(((uint64_t)matrix_array + (index * alignment)));
	*model_mat = new_matrix;



}

void World::fill_model_matrices(glm::mat4 *model_mat_array, uint32_t alignment) {
	// identity matrix for brushes
	set_model_matrix(
		model_mat_array, alignment,
		0, glm::mat4(1.f)
	);

	uint32_t i = 1;
	for (PropInstance<Collision::Cube> &cube: cubes) {
		set_model_matrix(
			model_mat_array, alignment,
			i, cube.get_matrix()
		);
		i++;
	}
}

void World::cmd_draw_indexed(RenderInfo &render_info) {
	for (SimpleBrush &brush: simple_brushes) {
		brush.cmd_draw_indexed(render_info);
	}

	uint32_t i = 1;
	for (PropInstance<Collision::Cube> &cube: cubes) {
		cube.cmd_draw_indexed(render_info, i);
		i++;
	}
}