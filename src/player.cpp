#include <cmath>
#include <vector>

#include <stdio.h>

#include "glm/gtc/matrix_transform.hpp"

#include "../include/player.hpp"

const float MOUSE_SPEED = 0.005f;
const float PLAYER_SPEED = 2.0f;

bool Player::collides(ColShape *other) {
	std::vector<glm::vec3> simplex;

	if (collider.ColShape::GJK(other, &simplex)) {
		glm::vec3 push_dir = collider.ColShape::EPA(other, simplex);

		this->position -= push_dir;
		this->model = glm::translate(this->model, -push_dir);
		this->view = glm::translate(this->view, -push_dir);

		return true;
	}

	return false;
}

CollisionSphere *Player::get_collider() {
	return &collider;
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
		std::sin(	(float)(theta - M_PI/2.0f)),
		-std::cos(	(float)(theta - M_PI/2.0f)), 
		0
	);

	// Up vector
	glm::vec3 up = glm::cross( right, direction );

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
			position += up * delta * PLAYER_SPEED;
		}
		if (glfwGetKey( window, GLFW_KEY_Q) == GLFW_PRESS){
			position -= up * delta * PLAYER_SPEED;
		}
	}

	// // Gravity
	// if (!noclip) {
	// 	position 
	// }

	// collides

	model = glm::translate(glm::mat4(1), position);
	collider.ColShape::set_transform(model);

	view = glm::lookAt(
		position,
		position + direction,
		up
	);
}

glm::mat4 Player::get_model_matrix() {
	return model;
}

glm::mat4 Player::get_view_matrix() {
	return view;
}