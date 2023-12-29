#ifndef BRUSHES_HPP
#define BRUSHES_HPP

#include <vector>

#include <glm/glm.hpp>

#include "../include/collisionShapes.hpp"
#include "../include/vertex.hpp"
#include "../include/renderInfo.hpp"

class Brush {
	protected:
		uint32_t vertex_offset;
		uint32_t first_index;
		std::vector<int> descriptor_set_indices;

	public:
		std::string tex_name;

		virtual bool generate_buffers(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
		) = 0;
		void set_descriptor_sets(std::vector<int> &set_indices);
		void cmd_draw_indexed(RenderInfo &render_info, uint32_t uboDynamicIndex);
};

class SimpleBrush: public Brush {
	private:
		glm::vec3 min_point = glm::vec3();
		glm::vec3 max_point = glm::vec3();

		Collision::AABox collider;
	
	public:
		SimpleBrush(
			const glm::vec3 &min_point, const glm::vec3 &max_point,
			std::string tex_name
		);

		Collision::AABox *get_collider_p();

		bool generate_buffers(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
		) override;
};

class PrismBrush: public Brush {
	private:
		glm::vec3 min_point = glm::vec3();
		glm::vec3 max_point = glm::vec3();
		std::vector<glm::vec3> base;

		Collision::AAPrism collider;
	
	public:
		PrismBrush(
			const glm::vec3 &min_point, const glm::vec3 &max_point,
			const std::vector<glm::vec3> &base, std::string tex_name
		);

		Collision::AAPrism *get_collider_p();

		bool generate_buffers(
			std::vector<Vertex> *vertices,
			std::vector<uint32_t> *indices
		) override;
};

#endif //BRUSHES_HPP