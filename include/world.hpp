#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>

#include "camera.hpp"
#include "brushes.hpp"
#include "vertex.hpp"

class World {
	private:
		std::vector<SimpleBrush> simple_brushes;
		Camera camera;

	public:
		World();

		bool get_geometry(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
		);

		void get_matrices(GLFWwindow *window, glm::mat4 *proj, glm::mat4 *view);
		void cmd_draw_indexed(VkCommandBuffer &commandBuffer);
};

#endif //WORLD_HPP