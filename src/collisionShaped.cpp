#include <limits>
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

size_t get_closest_face(
	std::vector<glm::vec3> &vertices,
	std::vector<size_t> &faces,
	std::vector<glm::vec3> &normals,
	float &best_dist
) {
	size_t best_face;

	for (size_t i = 0; i < faces.size() / 3; i++) {
		glm::vec3 A = vertices[faces[3 * i]];
		glm::vec3 B = vertices[faces[3 * i + 1]];
		glm::vec3 C = vertices[faces[3 * i + 2]];

		glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));
		float dist = glm::dot(A, normal);

		if (dist < 0) {
			dist = -dist;
			normal = -normal;
		}

		normals[i] = normal;

		if (dist < best_dist) {
			best_face = i;
			best_dist = dist;
		}
	}

	return best_face;
}

std::vector<size_t> get_new_faces(
	std::vector<glm::vec3> &vertices,
	std::vector<size_t> &faces,
	std::vector<glm::vec3> &normals,
	glm::vec3 &P
) {
	std::unordered_set<Edge, Edge::hash> edges;
	std::vector<size_t> new_faces;

	for (size_t i = 0; i < faces.size() / 3; i++) {
		glm::vec3 A = vertices[faces[3 * i]];

		if (glm::dot(normals[i], P) >= 0) {
			// this face is inside the new polyhedra
			// marking edges to be removed
			for (short j = 0; j < 3; j++) {
				Edge edge = {
					faces[3*i + j],
					faces[3*i + (j+1)%3]
				};

				if (!edges.insert(edge).second) {
					// if an edge is "removed" twice, then it's inside the new polyhedra
					edges.erase(edge);
				}
			}
		}

		else {
			// we keep this face
			new_faces.insert(new_faces.end(), {
				faces[3 * i],
				faces[3 * i + 1],
				faces[3 * i + 2],
			});
		}
	}

	// adding new faces:
	size_t i_P = vertices.size();
	vertices.push_back(P);

	for (const Edge &edge: edges) {
		new_faces.insert(new_faces.end(), {
			i_P,
			edge.a,
			edge.b
		});
	}

	return new_faces;
}

glm::vec3 ColShape::EPA(ColShape *other, std::vector<glm::vec3> &vertices) {
	std::vector<size_t> faces = {
		0, 1, 2,
		0, 1, 3,
		0, 2, 3,
		1, 2, 3
	};

	while(true) {
		
		// searching the face closest to the origin
		float best_dist = std::numeric_limits<float>::infinity();

		std::vector<glm::vec3> normals;
		normals.resize(faces.size() / 3);

		size_t best_face = get_closest_face(vertices, faces, normals, best_dist);

		glm::vec3 dir = normals[best_face];

		// special case : origin on closest face
		if (best_dist == 0) {
			glm::vec3 A = vertices[faces[3 * best_face]];
			glm::vec3 B;
			for (int i = 0; i < vertices.size(); i++) {
				B = vertices[i];
				if (glm::dot(dir, B - A) != 0) break;
			}
			// B is any vertex not on the face
			if (glm::dot(dir, B - A) > 0) dir = -dir;
		}

		// getting next support point
		glm::vec3 P = this->support_function(dir) - other->support_function(-dir);
		float dist = glm::dot(dir, P);
		printf("distance between face and point : %f\n", dist - best_dist);
		if (dist - best_dist <= EPSILON) return (dist + EPSILON) * dir;	// end
		
		// extending polyhedra to P
		auto new_faces = get_new_faces(vertices, faces, normals, P);

		faces.swap(new_faces);
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