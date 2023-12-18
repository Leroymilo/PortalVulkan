#include <unordered_set>
#include <vector>

#include <iostream>

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

	// second point
	if (A == O) {
		// edge case
		dir = -dir;
	}
	else {
		dir = glm::normalize(O - A);
	}
	B = this->support_function(dir) - other->support_function(-dir);
	if (glm::dot(B, dir) < 0) return false;	// new point did not pass the origin

	// third point
	glm::vec3 AB = B - A;
	glm::vec3 ortho = glm::cross(AB, O - A);
	if (ortho.length() == 0) {
		// edge case : origin on line
		ortho = glm::cross(AB, glm::vec3(AB.x, -AB.y, AB.z));
	}
	dir = glm::normalize(glm::cross(ortho, AB));
	
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
	float best_dist = FLT_MAX;
	size_t best_face = 0;

	for (int i = normals.size(); i < faces.size() / 3; i++) {
		size_t f = 3 * i;
		glm::vec3 A = vertices[faces[f]];
		glm::vec3 B = vertices[faces[f+1]];
		glm::vec3 C = vertices[faces[f+2]];

		glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));
		float dist = glm::dot(A, normal);

		if (dist < 0) {
			dist = -dist;
			normal = -normal;
		}

		normals.emplace_back(normal, dist);

		if (dist < best_dist) {
			best_dist = dist;
			best_face = i;
		}
	}

	return best_face;
}

glm::vec4 ColShape::EPA(ColShape *other, std::vector<glm::vec3> &vertices) {
	std::vector<size_t> faces = {
		0, 1, 2,
		0, 1, 3,
		0, 2, 3,
		1, 2, 3
	};

	std::vector<glm::vec4> normals;
	size_t best_face = get_normals(vertices, faces, normals);
	float best_dist = FLT_MAX;

	while (best_dist == FLT_MAX) {
		best_dist = normals[best_face].w;
		glm::vec3 best_norm = normals[best_face];

		// special case : origin on closest face
		if (best_dist < 10 * EPSILON) {
			std::cout << "special case" << std::endl;
			glm::vec3 dir = normals[best_face];
			glm::vec3 A = vertices[faces[3 * best_face]];
			glm::vec3 B;
			for (size_t f = 0; f < faces.size(); f++) {
				B = vertices[faces[f]];
				if (A == B) continue;
				if (glm::dot(dir, glm::normalize(B - A)) > EPSILON) break;
			}
			// B is any vertex not on the face
			if (glm::dot(dir, B - A) > 0) {
				normals[best_face] = glm::vec4(-dir, 0);
				best_norm = -dir;
			};
		}

		glm::vec3 P = this->support_function(best_norm) - other->support_function(-best_norm);
		float P_dist = glm::dot(best_norm, P);

		size_t f = 3 * best_face;

		std::cout << "face :" << std::endl;
		std::cout << "\t" << vertices[faces[f+0]].x << ", " << vertices[faces[f+0]].y << ", " << vertices[faces[f+0]].z << std::endl;
		std::cout << "\t" << vertices[faces[f+1]].x << ", " << vertices[faces[f+1]].y << ", " << vertices[faces[f+1]].z << std::endl;
		std::cout << "\t" << vertices[faces[f+2]].x << ", " << vertices[faces[f+2]].y << ", " << vertices[faces[f+2]].z << std::endl;
		std::cout << "normal : " << best_norm.x << ", " << best_norm.y << ", " << best_norm.z << std::endl;

		if (std::abs(P_dist - best_dist) > EPSILON) {
			best_dist = FLT_MAX;

			// extending polyhedra to P

			// removing faces inside the extended polyhedra
			std::unordered_set<Edge, Edge::hash> unique_edges;
			for (size_t i = 0; i < normals.size(); i++) {
				glm::vec3 n = normals[i];
				if (glm::dot(n, P) > normals[i].w) {
					// face is inside the new polyhedra

					// adding edges to unique_edges if unique (duh)
					for (int j = 0; j < 3; j++) {
						Edge edge = {faces[3*i + j], faces[3*i + (j+1)%3]};
						if (!unique_edges.insert(edge).second) {
							unique_edges.erase(edge);
						}
					}

					// removing face and its normal from polyhedra
					for (int j = 2; j >= 0; j--) {
						faces[3*i + j] = faces.back(); faces.pop_back();
					}
					normals[i] = normals.back(); normals.pop_back();
				}
			}

			size_t old_best_face = 0;
			for (size_t i = 0; i < normals.size(); i++) {
				if (normals[i].w < normals[old_best_face].w) {
					old_best_face = i;
				}
			}

			// adding new faces
			size_t i_P = vertices.size();
			vertices.push_back(P);
			for (const Edge &edge : unique_edges) {
				faces.insert(faces.end(), {
					edge.a,
					edge.b,
					i_P
				});
			}

			best_face = get_normals(vertices, faces, normals);

			if (normals[old_best_face].w < normals[best_face].w) {
				best_face = old_best_face;
			}
		}
	}

	return normals[best_face];
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
	float best_score = -FLT_MAX;

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

	// std::cout << "dir global : " << dir.x << ", " << dir.y << ", " << dir.z << std::endl;

	dir = glm::inverse(matrix) * glm::vec4(dir, 0);

	// std::cout << "dir local : " << dir.x << ", " << dir.y << ", " << dir.z << std::endl;

	glm::vec3 best_point;
	float best_score = -FLT_MAX;

	for (uint i = 0; i < 8; i++) {
		glm::vec3 point(size/2);
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

	// std::cout << "point local : " << best_point.x << ", " << best_point.y << ", " << best_point.z << std::endl;
	
	best_point = matrix * glm::vec4(best_point, 1);

	// std::cout << "point global : " << best_point.x << ", " << best_point.y << ", " << best_point.z << std::endl;

	return best_point;
}