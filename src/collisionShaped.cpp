#include <unordered_set>
#include <vector>

#include <stdio.h>

#include "../include/collisionShapes.hpp"

#define O glm::vec3(0, 0, 0)


const float EPSILON = 0.001;


// Required to make std::unordered_set<Edge>

bool Edge::operator==(const Edge &other) const {
	return (
		(this->a == other.a && this->b == other.b) ||
		(this->a == other.b && this->b == other.a)
	);
}

size_t Edge::hash::operator()(const Edge &edge) const {
	size_t a = edge.a, b = edge.b;
	return a >= b ? a * a + a + b : a + b * b;
	// from https://stackoverflow.com/a/13871379
}


// Any shape

void ColShape::set_transform(glm::mat4 new_matrix) {
	matrix = new_matrix;
}

bool ColShape::GJK(ColShape *other, std::vector<glm::vec3> *simplex) {
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
		simplex->assign({A, B, C, D});
		return true;
	}
}

size_t get_normals(
	std::vector<glm::vec3> &vertices,
	std::vector<size_t> &faces,
	std::vector<glm::vec4> &normals
) {
	size_t best_face;
	float best_dist = FLT_MAX;

	for (size_t i = normals.size() * 3; i < faces.size(); i+=3) {
		glm::vec3 A = vertices[faces[i]];
		glm::vec3 B = vertices[faces[i + 1]];
		glm::vec3 C = vertices[faces[i + 2]];

		glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));
		float dist = glm::dot(A, normal);

		if (dist < 0) {
			dist *= -1;
			normal *= -1;
		}

		// emplace uses glm::vec4 constructor
		normals.emplace_back(normal, dist);

		if (dist < best_dist) {
			best_face = i / 3;
			best_dist = dist;
		}
	}


	// special case : origin on closest face
	if (best_dist == 0) {
		glm::vec3 best_norm = normals[best_face];
		glm::vec3 A = vertices[faces[3 * best_face]];
		glm::vec3 B;
		for (int i = 0; i < faces.size(); i++) {
			B = vertices[faces[i]];
			if (glm::dot(best_norm, B - A) != 0) break;
		}
		// B is any vertex not on the face
		if (glm::dot(best_norm, B - A) > 0) normals[best_face] = glm::vec4(-best_norm, best_dist);
	}

	return best_face;
}

void expand_to_P(
	std::vector<glm::vec3> &vertices,
	std::vector<size_t> &faces,
	std::vector<glm::vec4> &normals,
	glm::vec3 &P
) {
	std::unordered_set<Edge, Edge::hash> unique_edges;

	for (size_t i = 0; i < normals.size(); i++) {

		if (glm::dot(glm::vec3(normals[i]), P) >= 0) {
			// this face is inside the new polyhedra
			// marking edges to be removed
			size_t f = i * 3;
			for (short j = 0; j < 3; j++) {
				Edge edge = {
					faces[f + j],
					faces[f + (j+1)%3]
				};

				if (!unique_edges.insert(edge).second) {
					// if an edge is "removed" twice, then it's inside the new polyhedra
					unique_edges.erase(edge);
				}

				faces[f + 2 - j] = faces.back(); faces.pop_back();
			}

			normals[i] = normals.back(); normals.pop_back();

			i--;
		}
	}

	// adding new faces:
	size_t i_P = vertices.size();
	vertices.push_back(P);

	for (const Edge &edge: unique_edges) {
		faces.insert(faces.end(), {
			i_P,
			edge.a,
			edge.b
		});
	}
}

glm::vec4 ColShape::EPA(ColShape *other, std::vector<glm::vec3> &vertices) {
	// adapted from https://www.youtube.com/watch?v=0XQ2FSz3EK8

	std::vector<size_t> faces = {
		0, 1, 2,
		0, 1, 3,
		0, 2, 3,
		1, 2, 3
	};

	// computing first normals
	std::vector<glm::vec4> normals;
	size_t best_face = get_normals(vertices, faces, normals);

	glm::vec3 dir;
	float best_dist = FLT_MAX;

	while(best_dist == FLT_MAX) {

		dir 		= normals[best_face];
		best_dist 	= normals[best_face].w;

		// getting next support point
		glm::vec3 P = this->support_function(dir) - other->support_function(-dir);
		float P_dist = glm::dot(dir, P);
		
		if (abs(P_dist - best_dist) > EPSILON) {
			best_dist = FLT_MAX;

			expand_to_P(vertices, faces, normals, P);

			float old_best_dist = FLT_MAX;
			for (size_t i = 0; i < normals.size(); i++) {
				if (normals[i].w < old_best_dist) {
					best_face = i;
					old_best_dist = normals[i].w;
				}
			}

			size_t new_best_face = get_normals(vertices, faces, normals);

			if (normals[new_best_face].w < old_best_dist) {
				best_face = new_best_face;
			}
		}
	}

	return glm::vec4(dir, best_dist + EPSILON);
}


// Sphere

CollisionSphere::CollisionSphere(float radius): radius(radius) {}

glm::vec3 CollisionSphere::support_function(glm::vec3 dir) {
	// Direction must be normalized

	glm::vec3 center = matrix * glm::vec4(0, 0, 0, 1);
	return center + dir * radius;
}


// Axis Aligned Box

CollisionAAB::CollisionAAB(glm::vec3 min_point, glm::vec3 max_point):
	min_point(min_point), max_point(max_point) {}

glm::vec3 CollisionAAB::support_function(glm::vec3 dir) {
	// Direction must be normalized

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

		float score = dot(dir, point);
		if (score > best_score) {
			best_score = score;
			best_point = point;
		}
	}

	return best_point;
}


// Cube

CollisionCube::CollisionCube(float size): size(size) {};

glm::vec3 CollisionCube::support_function(glm::vec3 dir) {
	// Direction must be normalized

	dir = glm::inverse(matrix) * glm::vec4(dir, 0);

	glm::vec3 best_point(size/2);
	float best_score = -std::numeric_limits<float>::infinity();

	for (uint i = 0; i < 8; i++) {
		glm::vec3 point;
		for (uint d = 0; d < 3; d++) {
			if (!((i >> d) & 1)) {
				point[d] *= -1;
			}
		}

		float score = dot(dir, point);
		if (score > best_score) {
			best_score = score;
			best_point = point;
		}
	}

	return best_point;
}