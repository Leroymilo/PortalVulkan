#include <unordered_set>
#include <vector>

#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "../include/collisionShapes.hpp"

#define O glm::vec3(0, 0, 0)
#define assertm(exp, msg) assert(((void)msg, exp))

using namespace Collision;

const float EPSILON = 0.001;

template <typename T> int signum(T val) {
    return (T(0) < val) - (val < T(0));
}


// Constructors ===================================================================================

PointShape::PointShape() {
	vertices.clear();
	vertices.emplace_back(0, 0, 0);
}

PointShape::PointShape(const std::vector<glm::vec3> &points): vertices(points) {
	assertm(vertices.size() > 0, "No points given for Vertices based shape");
}

template <class BaseShape>
SmoothShape<BaseShape>::SmoothShape() {
	base_shape = BaseShape();
	radius = 1;
}

template <class BaseShape>
SmoothShape<BaseShape>::SmoothShape(const BaseShape base_shape, float radius):
	base_shape(base_shape), radius(radius)
{
	assertm(radius > 0, "Invalid radius for SmoothShape (<= 0)");
}

AABox::AABox(const glm::vec3 &min_point, const glm::vec3 &max_point) {
	this->min_point = min_point;
	this->max_point = max_point;

	vertices.clear();
	
	for (uint i = 0; i < 8; i++) {
		glm::vec3 point;
		for (uint d = 0; d < 3; d++) {
			if ((i >> d) & 1) point[d] = max_point[d];
			else point[d] = min_point[d];
		}
		vertices.push_back(point);
	}
}

AAPrism::AAPrism(
	const glm::vec3 &min_point, const glm::vec3 &max_point,
	const std::vector<glm::vec3> &base
) {
	for (const glm::vec3 &point: base) {
		vertices.insert(
			vertices.end(), {
				min_point + point,
				max_point + point
			}
		);
	}

	// TODO : add center and test
}

Cube::Cube(float size): AABox(glm::vec3(-size/2.f), glm::vec3(size/2.f)) {}

Sphere::Sphere(float radius): SmoothShape<PointShape>(
	PointShape(),
	radius
) {}

Capsule::Capsule(float height, float radius): SmoothShape<PointShape>(
	PointShape({
		glm::vec3(0, 0,  height/2.f),
		glm::vec3(0, 0, -height/2.f)
	}),
	radius
) {
	assertm(height > 0, "Invalid height for Capsule (<= 0)");
}

Cylinder::Cylinder(float height, float radius): height(height), radius(radius) {
	shape_a = AABox(
		glm::vec3(-radius, -radius, -height/2),
		glm::vec3( radius,  radius,  height/2)
	);
	shape_b = Capsule(height, radius);
}


// Setters and Getters

void Shape::set_transform(glm::mat4 new_matrix) {
	matrix = new_matrix;
}

glm::vec3 Shape::get_center() {
	return glm::vec3(matrix * glm::vec4(center, 1.f));
}

void PointShape::print_points() {
	for (glm::vec3 p : vertices) {
		p = matrix * glm::vec4(p, 1);
		std::cout << "\t" << p.x << ", " << p.y << ", " << p.z << std::endl;
	}
}

template<>
void SmoothShape<PointShape>::set_base_transform(glm::mat4 new_matrix) {
	base_shape.set_transform(new_matrix);
}

template<class BaseShape>
BaseShape *SmoothShape<BaseShape>::get_base_shape() {
	return &base_shape;
}

template<class BaseShape>
float SmoothShape<BaseShape>::get_radius() {
	return radius;
}


// Support functions ==============================================================================

glm::vec3 PointShape::support(const glm::vec3 &dir) {
	// Direction must be normalized

	// O(vertices.size()), should be optimized for specific shapes when possible

	glm::vec3 local_dir = glm::inverse(matrix) * glm::vec4(dir, 0);
	glm::vec3 sum_best_points;
	uint nb_best_points;
	float best_score = -FLT_MAX;
	
	for (const glm::vec3 &vertex : vertices) {
		float score = glm::dot(local_dir, vertex);

		if (score > best_score) {
			best_score = score;
			sum_best_points = vertex;
			nb_best_points = 1;
		}

		else if (score == best_score) {
			sum_best_points += vertex;
			nb_best_points++;
		}
	}

	return matrix * glm::vec4(sum_best_points / (float)nb_best_points, 1);
}

glm::vec3 AABox::support(const glm::vec3 &dir) {
	// faster support function for AABox (instead of iterating over vertices)
	
	glm::vec3 local_dir = glm::inverse(matrix) * glm::vec4(dir, 0);
	glm::vec3 values[3] = {min_point, glm::vec3(0), max_point};

	glm::vec3 result;
	for (int i = 0; i < 3; i++) {
		result[i] = values[ signum(local_dir[i]) + 1 ][i];
	}

	return matrix * glm::vec4(result, 1);
}

template <class BaseShape>
glm::vec3 SmoothShape<BaseShape>::support(const glm::vec3 &dir) {
	// Direction must be normalized

	glm::vec3 base_support = base_shape.support(dir);
	return base_support + dir * radius;
}

glm::vec3 Cylinder::support(const glm::vec3 &dir) {
	// Direction must be normalized

	glm::vec3 local_dir = glm::inverse(matrix) * glm::vec4(dir, 0);
	glm::vec2 local_dir_xy = local_dir;

	if (local_dir_xy.length() > 0) {
		local_dir_xy = glm::normalize(local_dir_xy);
	}

	glm::vec4 local_support(
		radius * local_dir_xy,
		height * signum(local_dir.z),
		1
	);

	return matrix * local_support;
}


// Collision Detection ===========================================================================

glm::vec3 closest_on_line(std::vector<glm::vec3> *simplex) {
	// vertices
	glm::vec3 &A = (*simplex)[0];
	glm::vec3 &B = (*simplex)[1];
	glm::vec3 AB = B - A;
	if (glm::dot(AB, A) >= 0) {
		simplex->assign({A});
		return A;
	}
	if (glm::dot(AB, B) <= 0) {
		simplex->assign({B});
		return B;
	}

	// edge
	glm::vec3 d = glm::normalize(AB);
	return A - glm::dot(d, A) * d;
}

glm::vec3 closest_on_triangle(std::vector<glm::vec3> *simplex) {
	// vertices
	for (int i = 0; i < 3; i++) {
		glm::vec3 &A = (*simplex)[i];
		
		if (
			glm::dot(A, (*simplex)[(i+1)%3] - A) >= 0 &&
			glm::dot(A, (*simplex)[(i+2)%3] - A) >= 0
		) {
			simplex->assign({A});
			return A;
		}
	}

	glm::vec3 normal = glm::normalize(glm::cross(
		(*simplex)[1] - (*simplex)[0],
		(*simplex)[2] - (*simplex)[0]
	));

	// edges
	for (int i = 0; i < 3; i++) {
		glm::vec3 &A = (*simplex)[i];
		glm::vec3 &B = (*simplex)[(i+1)%3];

		if (glm::dot(glm::cross(normal, A - B), B) <= 0) {
			simplex->assign({A, B});
			return closest_on_line(simplex);
		}
	}

	// face
	return glm::dot(normal, (*simplex)[0]) * normal;
}

glm::vec3 closest(std::vector<glm::vec3> *simplex) {
	// finds point of simplex closest to origin
	// also reduces simplex to contain this point

	switch(simplex->size()) {
		case 1:
			return simplex->back();
		
		case 2:
			return closest_on_line(simplex);

		case 3:
			return closest_on_triangle(simplex);
	}

	// vertices
	for (int i = 0; i < 4; i++) {
		glm::vec3 &A = (*simplex)[i];

		if (
			glm::dot(A, (*simplex)[(i+1)%4] - A) >= 0 &&
			glm::dot(A, (*simplex)[(i+2)%4] - A) >= 0 &&
			glm::dot(A, (*simplex)[(i+3)%4] - A) >= 0
		) {
			simplex->assign({A});
			return A;
		}
	}

	// edges
	for (int i = 0; i < 4; i++) {
		glm::vec3 &A = (*simplex)[i];
		glm::vec3 &B = (*simplex)[(i+1)%4];
		glm::vec3 AC = (*simplex)[(i+2)%4] - A;
		glm::vec3 AD = (*simplex)[(i+3)%4] - A;

		glm::vec3 ABC = glm::cross(B - A, AC);
		glm::vec3 ABD = glm::cross(B - A, AD);

		if (
			glm::dot(ABC, A) * glm::dot(ABC, AD) >= 0 &&
			glm::dot(ABD, A) * glm::dot(ABD, AC) >= 0
		) {
			simplex->assign({A, B});
			return closest_on_line(simplex);
		}
	}

	// faces
	for (int i = 0; i < 4; i++) {
		glm::vec3 &A = (*simplex)[i];
		glm::vec3 &B = (*simplex)[(i+1)%4];
		glm::vec3 &C = (*simplex)[(i+2)%4];
		glm::vec3 AD = (*simplex)[(i+3)%4] - A;

		glm::vec3 ABC = glm::cross(B - A, C - A);

		if (glm::dot(ABC, A) * glm::dot(ABC, AD) >= 0) {
			simplex->assign({A, B, C});
			return closest_on_triangle(simplex);
		}
	}

	// in tetrahedron
	return O;
}

glm::vec3 support(Shape *shape_a, Shape *shape_b, const glm::vec3 &dir) {
	return shape_a->support(dir) - shape_b->support(-dir);
}

bool GJK(Shape *shape_a, Shape *shape_b, glm::vec4 *dir_dist, std::vector<glm::vec3> *simplex) {
	// Complete GJK algorithm https://youtu.be/DGVZYdlw_uo?si=ME8J8EsuHkM2NdFP
	// returns ( shape_a ∩ shape_b != ⌀ ), i.e. : true if shape_a and shape_b intersects

	simplex->clear();
	glm::vec3 v = support(shape_a, shape_b, glm::vec3(1, 0, 0));

	while (true) {
		if (glm::length(v) <= EPSILON) {
			// cannot normalize because v on origin
			// so point of simplex closest to origin is origin,
			// i.e. : origin in simplex
			*dir_dist = glm::vec4(1, 0, 0, 0);
			return true;
		}
		glm::vec3 dir = -glm::normalize(v);

		glm::vec3 w = support(shape_a, shape_b, dir);
		if (glm::dot(v, dir) + EPSILON >= glm::dot(w, dir)) {
			// w is no more extreme than v in direction
			*dir_dist = glm::vec4(dir, glm::length(v) + EPSILON);
			return false;
		}

		simplex->push_back(w);
		v = closest(simplex);
	}
}

bool GJK_fast(Shape *shape_a, Shape *shape_b, std::vector<glm::vec3> *simplex) {
	// GJK algorithm with shortcuts https://www.youtube.com/watch?v=ajv46BSqcK4
	// returns ( shape_a ∩ shape_b != ⌀ ), i.e. : true if shape_a and shape_b intersects

	glm::vec3 A, B, C, D;
	glm::vec3 dir = glm::vec3(1, 0, 0);

	// first point
	A = support(shape_a, shape_b, dir);

	// second point
	if (A == O) {
		// edge case
		dir = -dir;
	}
	else {
		dir = glm::normalize(O - A);
	}
	B = support(shape_a, shape_b, dir);
	if (glm::dot(B, dir) < 0) return false;	// new point did not pass the origin

	// third point
	glm::vec3 AB = B - A;
	glm::vec3 ortho = glm::cross(AB, O - A);
	if (ortho.length() == 0) {
		// edge case : origin on line
		ortho = glm::cross(AB, glm::vec3(AB.x, -AB.y, AB.z));
	}
	dir = glm::normalize(glm::cross(ortho, AB));
	
	C = support(shape_a, shape_b, dir);
	if (glm::dot(C, dir) < 0) return false;	// new point did not pass the origin

	while (true) {
		// fourth point
		dir = glm::normalize(glm::cross(B - A, C - A));
		if (glm::dot(dir, A) > 0) dir = -dir;	// wrong plane normal
		D = support(shape_a, shape_b, dir);
		if (glm::dot(D, dir) < 0) return false;	// new point did not pass the origin
		if (D == A || D == B || D == C) return false; // new point is one of previous points

		// Check if O in simplex ABCD
		dir = glm::cross(A - D, B - D);
		if (glm::dot(dir, C - D) * glm::dot(dir, D) > 0) {
			// Origin on wrong side of plane ABD
			std::swap(C, D);
			continue;
		}
		dir = glm::cross(B - D, C - D);
		if (glm::dot(dir, A - D) * glm::dot(dir, D) > 0) {
			// Origin on wrong side of plane BCD
			std::swap(A, D);
			continue;
		}
		dir = glm::cross(C - D, A - D);
		if (glm::dot(dir, B - D) * glm::dot(dir, D) > 0) {
			// Origin on wrong side of plane CAD
			std::swap(B, D);
			continue;
		}

		if (simplex != nullptr) {
			// Origin on the right side of all planes of simplex, so in simplex
			simplex->assign({A, B, C, D});
		}

		return true;
	}
}

bool Shape::is_colliding(Shape *other) {
	return GJK_fast(this, other, nullptr);
}


// Collision Resolution ===========================================================================

// Edge struct to simplify search of unique edges
struct Edge {
	size_t a;
	size_t b;

	bool operator==(const Edge &other) const {
		return (
			(this->a == other.a && this->b == other.b) ||
			(this->a == other.b && this->b == other.a)
		);
	}

	struct hash {
		size_t operator()(const Edge &edge) const {
			size_t a = std::min(edge.a, edge.b), b = std::max(edge.a, edge.b);
			return a + b * b;
		}
	};
};

size_t get_normals(
	std::vector<glm::vec3> &vertices,
	std::vector<size_t> &faces,
	std::vector<glm::vec4> &normals
) {
	float best_dist = FLT_MAX;
	size_t best_face = 0;

	for (int i = normals.size(); i < faces.size() / 3; i++) {
		size_t f = 3 * i;
		glm::vec3 &A = vertices[faces[f]];
		glm::vec3 &B = vertices[faces[f+1]];
		glm::vec3 &C = vertices[faces[f+2]];

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

glm::vec4 EPA(Shape *shape_a, Shape *shape_b, std::vector<glm::vec3> &vertices) {
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
		if (best_dist == 0) {
			glm::vec3 &A = vertices[faces[3 * best_face]];
			glm::vec3 B;
			for (size_t f = 0; f < faces.size(); f++) {
				B = vertices[faces[f]];
				if (A == B) continue;
				if (glm::dot(best_norm, glm::normalize(B - A)) != 0) break;
			}
			// B is any vertex not on the face
			if (glm::dot(best_norm, B - A) > 0) {
				best_norm *= -1;
				normals[best_face] = glm::vec4(best_norm, 0);
			};
		}

		glm::vec3 P = support(shape_a, shape_b, best_norm);
		float P_dist = glm::dot(best_norm, P);

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

					i--;
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

	normals[best_face].w += EPSILON;
	return normals[best_face];
}

glm::vec4 Collision::resolve(PointShape *shape_a, PointShape *shape_b) {
	std::vector<glm::vec3> vertices;

	if (!GJK_fast(shape_a, shape_b, &vertices)) {
		return glm::vec4(1, 0, 0, 0);	// no collision -> distance is 0
	}

	return EPA(shape_a, shape_b, vertices);
}

glm::vec4 Collision::resolve(SmoothShape<PointShape> *shape_a, PointShape *shape_b) {
	std::vector<glm::vec3> vertices;
	glm::vec4 dir_dist;

	if (!GJK_fast(shape_a, shape_b, nullptr)) {
		return glm::vec4(1, 0, 0, 0);	// no collision -> distance is 0
	}

	PointShape *base_shape = shape_a->get_base_shape();
	bool base_collides = GJK(base_shape, shape_b, &dir_dist, &vertices);
	if (base_collides) {
		// base shape collides with shape b
		// compute dir_dist for base_shape
		
		if(vertices.size() < 4) {
			// edge case, must ensure vertices.size() == 4 for EPA
			GJK_fast(base_shape, shape_b, &vertices);
		}

		dir_dist = EPA(base_shape, shape_b, vertices);
	}

	// add radius to dir_dist to apply smoothness of shape
	dir_dist.w = shape_a->get_radius() - dir_dist.w;
	return dir_dist;
}