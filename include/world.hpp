#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include "player.hpp"
#include "brushes.hpp"

class World {
	private:
		std::vector<SimpleBrush> simple_brushes;
		Player player;

	public:
		World();

		void get_geometry(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
		);
		size_t get_nb_objects();
		
		std::vector<std::string> get_tex_names();
		void set_descriptor_sets(std::unordered_map<std::string, std::vector<int>> &set_indices_map);

		void process_physics(GLFWwindow *window);
		void get_matrices(glm::mat4 *view);
		void cmd_draw_indexed(RenderInfo &render_info);
};

#endif //WORLD_HPP