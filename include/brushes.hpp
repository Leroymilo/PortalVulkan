#ifndef BRUSHES_HPP
#define BRUSHES_HPP

#include <glm/glm.hpp>

#include "../include/vertex.hpp"

class SimpleBrush {
	private:
		glm::vec3 min_point = glm::vec3();
		glm::vec3 max_point = glm::vec3();

		uint32_t vertex_offset;
		uint32_t first_index;
	
	public:
		SimpleBrush(glm::vec3 min_point, glm::vec3 max_point);

		bool generate_buffers(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
		);
		
		void cmd_draw_indexed(VkCommandBuffer &commandBuffer);
};

#endif //BRUSHES_HPP