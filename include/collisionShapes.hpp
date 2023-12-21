#ifndef COLLISION_SHAPES_HDD
#define COLLISION_SHAPES_HDD

#include "glm/glm.hpp"


// Utils for collision resolution

struct Edge {
	size_t a;
	size_t b;

	bool operator==(const Edge &other) const;

	struct hash {
		size_t operator()(const Edge &edge) const;
	};
};

size_t get_normals(std::vector<glm::vec3> &vertices, std::vector<size_t> &faces, std::vector<glm::vec4> &normals);


// Classes

namespace Collision {

	class Shape {
		protected:
			glm::mat4 matrix = glm::mat4(1.0f);
			virtual glm::vec3 support(glm::vec3 dir) = 0;
		
		public:
			void set_transform(glm::mat4 new_matrix);
			bool is_colliding(Shape *other, std::vector<glm::vec3> *simplex);
	};

	class PointShape: public Shape {
		// Basically a polyhedra defined by its vertices.

		protected:
			std::vector<glm::vec3> vertices;
		
		public:
			PointShape();	// Single point in (0, 0, 0)
			PointShape(const std::vector<glm::vec3> &points);

			glm::vec3 support(glm::vec3 dir) override;
	};

	template<class BaseShape>
	class SmoothShape: public Shape {
		// Defined as the Minkovski addition of a base Shape (of any specialisation) and a sphere centered in 0.

		static_assert(std::is_base_of<Shape, BaseShape>::value, "BaseShape is not derived from Shape");

		protected:
			BaseShape base_shape;
			float radius;
		
		public:
			SmoothShape();	// Circle of radius 1 centered in (0, 0, 0)
			SmoothShape(const BaseShape base_shape, float radius);

			glm::vec3 support(glm::vec3 dir) override;
	};

	template<class ShapeA, class ShapeB>
	class InterShape: public Shape {
		// Defined as the intersection of 2 Shapes.

		static_assert(std::is_base_of<Shape, ShapeA>::value, "ShapeA is not derived from Shape");
		static_assert(std::is_base_of<Shape, ShapeB>::value, "ShapeB is not derived from Shape");

		protected:
			ShapeA shape_a;
			ShapeB shape_b;
			virtual glm::vec3 support(glm::vec3 dir) = 0;
			// The support function must be defined for every InterShape sub-class,
			// it cannot be expressed from the support functions of the shapes it is an intersection of (afaIk)
	};

	// Classes to help define common shapes

	class AAB: public PointShape {
		public:
			AAB(glm::vec3 min_point, glm::vec3 max_point);
	};

	class Cube: public AAB {
		public:
			Cube(float size);
	};

	class Sphere: public SmoothShape<PointShape> {
		public:
			Sphere(float radius);
	};

	class Capsule: public SmoothShape<PointShape> {
		public:
			Capsule(float height, float radius);
	};

	class Cylinder: public InterShape<PointShape, SmoothShape<PointShape>> {
		private:
			float height, radius;

		public:
			Cylinder(float height, float radius);

			glm::vec3 support(glm::vec3 dir) override;
			// required for InterShape
	};


	// Collision resolution :

	glm::vec4 resolve(PointShape *shape_a, PointShape *shape_b);
	glm::vec4 resolve(SmoothShape<PointShape> *shape_a, PointShape *shape_b);
}

#endif //COLLISION_SHAPES_HDD