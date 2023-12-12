#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "collisionShapes.hpp"
#include "glm/glm.hpp"

class Player {
	private:
		CollisionSphere collider = CollisionSphere(0.5);
		bool noclip = true;

		glm::vec3 position = glm::vec3(0, 0, 1.8);
		float theta = 0;
		float phi = 0;

		glm::mat4 model;
		glm::mat4 view;

	public:
		bool collides(ColShape *other);
		void process_physics(GLFWwindow *window, float delta);
		glm::mat4 get_model_matrix();
		glm::mat4 get_view_matrix();
};

#endif //PLAYER_HPP