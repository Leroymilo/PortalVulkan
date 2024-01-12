#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "vertex.hpp"
#include "renderInfo.hpp"

class Model {

	private:
		std::string obj_name;

		uint32_t vertex_offset;
		uint32_t first_index;
		uint32_t index_count = 0;
		std::vector<int> descriptor_set_indices;

	public:
		std::string tex_name;

		Model(std::string obj_name, std::string tex_name);

		bool generate_buffers(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
		);
		void set_descriptor_sets(std::vector<int> &set_indices);
		void cmd_draw_indexed(RenderInfo &render_info, uint32_t ubo_dyna_index);
};

#endif //MODEL_HPP