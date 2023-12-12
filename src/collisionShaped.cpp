#include <limits>
#include <stdio.h>
#include <vector>

#include "../include/collisionShapes.hpp"

// Any shape

void ColShape::set_transform(glm::mat4 new_matrix) {
	matrix = new_matrix;
}

bool ColShape::collides(ColShape *other) {
	std::vector<glm::vec4> points;
	glm::vec4 dir = glm::vec4(1, 0, 0, 0);

	glm::vec4 point = this->support_function(dir) - other->support_function(-dir);
	point.w = 1;
	printf("point : %f, %f, %f, %f\n", point.x, point.y, point.z, point.w);
	points.push_back(point);

	dir = glm::normalize(glm::vec4(0, 0, 0, 1) - point);
	point = this->support_function(dir) - other->support_function(-dir);
	point.w = 1;
	printf("point : %f, %f, %f, %f\n", point.x, point.y, point.z, point.w);
	points.push_back(point);

	printf("\n");

	return true;
}


// Sphere

CollisionSphere::CollisionSphere(float radius): radius(radius) {}

glm::vec4 CollisionSphere::support_function(glm::vec4 direction) {
	// Direction must be normalized and with w == 0

	glm::vec4 center = matrix * glm::vec4(0, 0, 0, 1);
	return center + direction * radius;
}


// Axis Aligned Box

CollisionAAB::CollisionAAB(glm::vec4 min_point, glm::vec4 max_point):
	min_point(min_point), max_point(max_point) {}

glm::vec4 CollisionAAB::support_function(glm::vec4 direction) {
	// Direction must be normalized and with w == 0

	glm::vec4 best_point;
	float best_score = std::numeric_limits<float>::min();

	for (int i = 0; i < 8; i++) {
		glm::vec4 point = glm::vec4(0, 0, 0, 1);
		for (int d = 0; d < 3; d++) {
			if ((i >> d) && 1) {
				point[d] = max_point[d];
			}
			else {
				point[d] = min_point[d];
			}
		}

		float score = dot(direction, point);
		if (score > best_score) {
			best_score = score;
			best_point = point;
		}
	}

	return best_point;
}