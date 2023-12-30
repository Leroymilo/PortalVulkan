#ifndef PROPS_HPP
#define PROPS_HPP

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "vertex.hpp"
#include "renderInfo.hpp"
#include "collisionShapes.hpp"

class Props {
	private:
		std::string obj_name;

		uint32_t vertex_offset;
		uint32_t first_index;
		std::vector<int> descriptor_set_indices;

	public:
		std::string tex_name;

		bool generate_buffers(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
		);
		void set_descriptor_sets(std::vector<int> &set_indices);
		void cmd_draw_indexed(RenderInfo &render_info, uint32_t uboDynamicIndex);
};

class PropInstance {
	private:
		glm::mat4 matrix = glm::mat4(1.f);

	public:
		Props props_info;

		void cmd_draw_indexed(RenderInfo &render_info, uint32_t uboDynamicIndex);
};

class Cube : public PropInstance {
	private:
		Collision::Cube collider = Collision::Cube(0.2);

	public:
		Collision::AABox *get_collider_p();
};

#endif //PROPS_HPP