#ifndef CAMERA_HPP
#define CAMERA_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// free cam
class Camera {
	private:
		glm::vec3 position = glm::vec3( 0, 0, 1.8);
		float horizontalAngle = 3.14f;
		float verticalAngle = 0.0f;
		float initialFoV = 90.0f;

		float speed = 2.0f; // 3 units / second
		float mouseSpeed = 0.005f;

	public:
		void get_matrices(GLFWwindow *window, glm::mat4 *proj, glm::mat4 *view);
};

#endif //CAMERA_HPP