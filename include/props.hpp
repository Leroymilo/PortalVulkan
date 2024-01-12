#ifndef PROPS_HPP
#define PROPS_HPP

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "vertex.hpp"
#include "renderInfo.hpp"
#include "collisionShapes.hpp"
#include "model.hpp"


template <class Collider>
class PropInstance {

	static_assert(
		std::is_base_of<Collision::Shape, Collider>::value,
		"Collider is not derived from Shape"
	);

	private:
		glm::mat4 matrix = glm::mat4(1.f);

	protected:
		Collider collider;
		Model *model;

	public:
		PropInstance(Model *model, Collider collider);

		const glm::mat4 &get_matrix();
		Collider *get_collider_p();
		void process_physics(float delta);
		void nudge(const glm::vec4 &dir_dist);
		void cmd_draw_indexed(RenderInfo &render_info, uint32_t ubo_dyna_index);
};

class Cube : public PropInstance<Collision::Cube> {

	public:
		Cube(Model *model, float size);
};

#endif //PROPS_HPP