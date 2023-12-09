#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>

#include "brushes.hpp"
#include "vertex.hpp"

class World {
	private:
		std::vector<SimpleBrush> simple_brushes;

	public:
		World();

		bool get_geometry(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
		);

		void cmd_draw_indexed(VkCommandBuffer &commandBuffer);
};

#endif //WORLD_HPP