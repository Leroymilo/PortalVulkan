#ifndef PLAYER_HPP
#define PLAYER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "collisionShapes.hpp"

// #define M_PI 3.14159265358979323846	//because VSCode can't read include again...

class Player {
	private:
		CollisionCube collider = CollisionCube(0.2);
		bool noclip = true;

		glm::vec3 position = glm::vec3(0, 0, 1.8);
		float theta = 0;
		float phi = 0;

		glm::mat4 model;
		glm::mat4 view;

		void compute_transforms();

	public:
		void process_physics(GLFWwindow *window, float delta);
		ColShape *get_collider();
		bool collides(ColShape *other);
		glm::mat4 get_model_matrix();
		glm::mat4 get_view_matrix();
};

#endif //PLAYER_HPP