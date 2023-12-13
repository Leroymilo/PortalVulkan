#include <limits>
#include <vector>

#include "../include/collisionShapes.hpp"


const glm::vec3 O = glm::vec3(0, 0, 0);


// Any shape

void ColShape::set_transform(glm::mat4 new_matrix) {
	matrix = new_matrix;
}

bool ColShape::collides(ColShape *other) {
	glm::vec3 A, B, C, D;
	glm::vec3 dir = glm::vec3(1, 0, 0);

	// first point
	A = this->support_function(dir) - other->support_function(-dir);
	if (A == O) return true; // edge case

	// second point
	dir = glm::normalize(O - A);
	B = this->support_function(dir) - other->support_function(-dir);
	if (glm::dot(B, dir) < 0) return false;	// new point did not pass the origin

	// third point
	glm::vec3 ortho = glm::cross(B - A, O - A);
	if (ortho.length() == 0) return true;	// edge case : origin on line
	dir = glm::normalize(glm::cross(ortho, B - A));
	C = this->support_function(dir) - other->support_function(-dir);
	if (glm::dot(C, dir) < 0) return false;	// new point did not pass the origin

	while (true) {
		// fourth point
		dir = glm::normalize(glm::cross(B - A, C - A));
		if (glm::dot(dir, A) > 0) dir = -dir;	// wrong plane normal
		D = this->support_function(dir) - other->support_function(-dir);
		if (glm::dot(D, dir) < 0) return false;	// new point did not pass the origin
		if (D == A || D == B || D == C) return false; // new point is one of previous points

		// Check if O in simplex ABCD
		dir = glm::cross(A - D, B - D);
		if (glm::dot(dir, C) * glm::dot(dir, D) > 0) {
			// Origin on wrong side of plane ABD
			std::swap(C, D);
			continue;
		}
		dir = glm::cross(B - D, C - D);
		if (glm::dot(dir, A) * glm::dot(dir, D) > 0) {
			// Origin on wrong side of plane BCD
			std::swap(A, D);
			continue;
		}
		dir = glm::cross(C - D, A - D);
		if (glm::dot(dir, B) * glm::dot(dir, D) > 0) {
			// Origin on wrong side of plane CAD
			std::swap(B, D);
			continue;
		}

		// Origin on the right side of all planes of simplex, so in simplex
		return true;
	}
}


// Sphere

CollisionSphere::CollisionSphere(float radius): radius(radius) {}

glm::vec3 CollisionSphere::support_function(glm::vec3 direction) {
	// Direction must be normalized and with w == 0

	glm::vec3 center = matrix * glm::vec4(0, 0, 0, 1);
	return center + direction * radius;
}


// Axis Aligned Box

CollisionAAB::CollisionAAB(glm::vec3 min_point, glm::vec3 max_point):
	min_point(min_point), max_point(max_point) {}

glm::vec3 CollisionAAB::support_function(glm::vec3 direction) {
	// Direction must be normalized and with w == 0

	glm::vec3 best_point;
	float best_score = -std::numeric_limits<float>::infinity();

	for (uint i = 0; i < 8; i++) {
		glm::vec3 point;
		for (uint d = 0; d < 3; d++) {
			if ((i >> d) & 1) {
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