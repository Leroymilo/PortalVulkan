#ifndef BRUSHES_HPP
#define BRUSHES_HPP

#include <vector>

#include <glm/glm.hpp>

#include "../include/collisionShapes.hpp"
#include "../include/vertex.hpp"
#include "../include/renderInfo.hpp"

class SimpleBrush {
	private:
		glm::vec3 min_point = glm::vec3();
		glm::vec3 max_point = glm::vec3();

		CollisionAAB collider;

		uint32_t vertex_offset;
		uint32_t first_index;

		std::vector<int> descriptor_set_indices;
	
	public:
		std::string tex_name;

		SimpleBrush(glm::vec3 min_point, glm::vec3 max_point, std::string tex_name);

		CollisionAAB *get_collider_p();

		bool generate_buffers(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
		);

		void set_descriptor_sets(std::vector<int> &set_indices);
		
		void cmd_draw_indexed(RenderInfo &render_info);
};

#endif //BRUSHES_HPP