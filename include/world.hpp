#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include "player.hpp"
#include "brushes.hpp"
#include "model.hpp"
#include "props.hpp"

class World {
	private:
		std::vector<SimpleBrush> simple_brushes;
		std::vector<Model> models;
		std::vector<Cube> cubes;
		Player player;

		void set_model_matrix(
			glm::mat4 *matrix_array,
			uint32_t alignment, uint32_t index,
			const glm::mat4 &new_matrix
		);

	public:
		World();

		void get_geometry(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
		);
		size_t count_dyna_ubo();
		
		std::vector<std::string> get_tex_names();
		void set_descriptor_sets(std::unordered_map<std::string, std::vector<int>> &set_indices_map);

		void process_physics(GLFWwindow *window);
		void get_view(glm::mat4 *view);
		void fill_model_matrices(glm::mat4 *model_mat_array, uint32_t alignment);
		void cmd_draw_indexed(RenderInfo &render_info);
};

#endif //WORLD_HPP