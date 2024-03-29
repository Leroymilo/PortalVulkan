#include <cmath>
#include <vector>

#include <iostream>

#include "glm/gtc/matrix_transform.hpp"

#include "../include/player.hpp"

const float MOUSE_SPEED = 0.005f;
const float PLAYER_SPEED = 2.0f;

void Player::compute_transforms() {

	glm::vec3 direction(
		sin(theta),
		cos(theta),
		0
	);

	model = glm::inverse(glm::lookAt(position, position + direction, glm::vec3(0, 0, 1)));
	collider.set_base_transform(model);

	direction = glm::vec3(
		std::cos(phi) * std::sin(theta),
		- std::cos(phi) * std::cos(theta), 
		std::sin(phi)
	);

	glm::vec3 up(
		- std::sin(phi) * std::sin(theta),
		std::sin(phi) * std::cos(theta),
		std::cos(phi) * std::abs(std::cos(2 * theta))
	);

	view = glm::lookAt(position, position + direction, up);
}

void Player::process_physics(GLFWwindow *window, float delta) {
	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, width/2, height/2);

	theta 	+= MOUSE_SPEED * float(width  / 2 - xpos);
	phi 	+= MOUSE_SPEED * float(height / 2 - ypos);
	phi = std::min(phi,  (float)M_PI/2);
	phi = std::max(phi, -(float)M_PI/2);

	glm::vec3 direction;

	if (noclip) {
		direction = glm::vec3(
			std::cos(phi) * std::sin(theta),
			- std::cos(phi) * std::cos(theta), 
			std::sin(phi)
		);
	}
	else {
		direction  = glm::vec3(
			std::sin(theta),
			std::cos(theta),
			0
		);
	}

	glm::vec3 right = glm::vec3(
		-std::cos(theta),
		-std::sin(theta), 
		0
	);


	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		position += direction * delta * PLAYER_SPEED;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position -= direction * delta * PLAYER_SPEED;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * delta * PLAYER_SPEED;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * delta * PLAYER_SPEED;
	}

	// Flying
	if (noclip) {
		if (glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS){
			position += glm::vec3(0, 0, 1) * delta * PLAYER_SPEED;
		}
		if (glfwGetKey( window, GLFW_KEY_Q) == GLFW_PRESS){
			position -= glm::vec3(0, 0, 1) * delta * PLAYER_SPEED;
		}
	}

	// // Gravity
	// if (!noclip) {
	// 	position 
	// }

	compute_transforms();
}

Collision::Sphere *Player::get_collider_p() {
	return &collider;
}

void Player::nudge(const glm::vec4 &dir_dist) {
	position -= glm::vec3(dir_dist) * dir_dist.w;
	compute_transforms();
}

glm::mat4 Player::get_model_matrix() {
	return model;
}

glm::mat4 Player::get_view_matrix() {
	return view;
}