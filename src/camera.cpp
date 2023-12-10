
#include <glm/gtc/matrix_transform.hpp>

#include "../include/camera.hpp"

void Camera::get_matrices(GLFWwindow *window, glm::mat4 *proj, glm::mat4 *view) {
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	uint16_t width = 800;
	uint16_t height = 600;

	// Reset mouse position for next frame
	glfwSetCursorPos(window, width/2, height/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(width/2 - xpos );
	verticalAngle   += mouseSpeed * float( height/2 - ypos );
	verticalAngle = std::min(verticalAngle, 3.14f/2);
	verticalAngle = std::max(verticalAngle, -3.14f/2);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		std::cos(verticalAngle) * std::sin(horizontalAngle),
		- std::cos(verticalAngle) * std::cos(horizontalAngle), 
		std::sin(verticalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		std::sin(horizontalAngle - 3.14f/2.0f),
		- std::cos(horizontalAngle - 3.14f/2.0f), 
		0
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	*proj = glm::perspective(glm::radians(FoV), width / float(height), 0.01f, 100.0f);
	// Camera matrix
	*view = glm::lookAt(
		position,           // Camera is here
		position+direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}