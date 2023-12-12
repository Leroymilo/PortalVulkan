#ifndef WORLD_HPP
#define WORLD_HPP

#include <unordered_set>
#include <tuple>
#include <vector>

#include "player.hpp"
#include "brushes.hpp"
#include "vertex.hpp"

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
		
		std::vector<std::string> get_tex_names();
		void set_descriptor_sets(std::unordered_map<std::string, std::vector<int>> &set_indices_map);

		void process_physics(GLFWwindow *window);
		void get_matrices(glm::mat4 *view);
		void cmd_draw_indexed(RenderInfo &render_info);
};

namespace std {
	template<> struct hash<tuple<string, string>> {
		size_t operator()(tuple<string, string> const& tex_comb) const {
			size_t result = hash<string>() ( get<0> (tex_comb) );
			result = result ^ (hash<string>() ( get<0> (tex_comb) ) << 1);
			return result;
		}
	};
}


#endif //WORLD_HPP