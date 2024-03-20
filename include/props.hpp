#ifndef PROPS_HPP
#define PROPS_HPP

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "vertex.hpp"
#include "renderInfo.hpp"
#include "collisionShapes.hpp"
#include "model.hpp"

struct Physics {
	float mass;		// in grams
	float inertia;	// in 

	glm::vec3 position = glm::vec3(0.f);
	glm::vec3 rotation = glm::vec3(0.f);
	
	glm::vec3 speed = glm::vec3(0.f);
	glm::vec3 rot_speed = glm::vec3(0.f);

	glm::mat4 get_matrix();
};

template <class Collider>
class PropInstance {

	static_assert(
		std::is_base_of<Collision::Shape, Collider>::value,
		"Collider is not derived from Shape"
	);

	private:
		Physics physics_props = {5, 5};
		Physics prev_ph_props = physics_props;

		// glm::vec3 position = glm::vec3(0.f);
		// glm::vec3 rotation = glm::vec3(0.f);
		// glm::mat4 matrix = glm::mat4(1.f);

		// float mass = 5;	// in grams
		// glm::vec3 speed = glm::vec3(0.f);
		// glm::vec3 rot_speed = glm::vec3(0.f);

		std::vector<glm::vec3> contact_points;

		void set_matrix(const glm::mat4 &new_matrix);

	protected:
		Collider collider;
		Model *model;

	public:
		PropInstance(Model *model, Collider collider);

		const glm::mat4 get_matrix();
		void set_rot_pos(const glm::vec3 &new_pos, const glm::vec3 &new_rot);
		Collider *get_collider_p();
		void process_physics(float delta);
		void resolve(const glm::vec4 &dir_dist, float delta);
		void cmd_draw_indexed(RenderInfo &render_info, uint32_t ubo_dyna_index);
};

class Cube : public PropInstance<Collision::Cube> {

	public:
		Cube(Model *model, float size);
};

#endif //PROPS_HPP