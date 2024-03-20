#ifndef PLAYER_HPP
#define PLAYER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "collisionShapes.hpp"

// #define M_PI 3.14159265358979323846	//because VSCode can't read include again...

class Player {
	private:
		Collision::Sphere collider = Collision::Sphere(0.1);
		bool noclip = true;

		glm::vec3 position = glm::vec3(0, 2, 1);
		float theta = 0;
		float phi = 0;

		glm::mat4 model;
		glm::mat4 view;

		void compute_transforms();

	public:
		Collision::Sphere *get_collider_p();
		glm::mat4 get_model_matrix();
		glm::mat4 get_view_matrix();
		void process_physics(GLFWwindow *window, float delta);
		void nudge(const glm::vec4 &dir_dist);
};

#endif //PLAYER_HPP