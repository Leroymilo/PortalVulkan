#include <cmath>

#include "../include/player.hpp"

const float MOUSE_SPEED = 0.005f;
const float PLAYER_SPEED = 2.0f;

bool Player::collides(ColShape *other) {
	return collider.ColShape::collides(other);
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
		std::sin(	theta - M_PI/2.0f),
		-std::cos(	theta - M_PI/2.0f), 
		0
	);

	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * delta * PLAYER_SPEED;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * delta * PLAYER_SPEED;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * delta * PLAYER_SPEED;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * delta * PLAYER_SPEED;
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