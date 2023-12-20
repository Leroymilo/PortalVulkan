#ifndef COLLISION_SHAPES_HDD
#define COLLISION_SHAPES_HDD

#include "glm/glm.hpp"


// Functions

size_t get_normals(std::vector<glm::vec3> &vertices, std::vector<size_t> &faces, std::vector<glm::vec4> &normals);


// Structs

struct Edge {
	size_t a;
	size_t b;

	bool operator==(const Edge &other) const;

	struct hash {
		size_t operator()(const Edge &edge) const;
	};
};


// Classes

class ColShape {
	protected:
		glm::mat4 matrix = glm::mat4(1.0f);
	
	public:
		void set_transform(glm::mat4 new_matrix);
		virtual glm::vec3 support(glm::vec3 dir) = 0;
		bool is_colliding(ColShape *other, std::vector<glm::vec3> *simplex);
};

class ColSharpShape;
class ColSmoothShape;


// Sharp Shapes ==============================================================================================

class ColSharpShape: public ColShape {
	public:
		virtual void get_faces(std::vector<glm::vec4> *faces) = 0;
		glm::vec4 resolve_collision(ColSharpShape *other, std::vector<glm::vec3> &vertices);
		glm::vec4 resolve_collision(ColSmoothShape *other);
};

class CollisionAAB: public ColSharpShape {
	// /!\ this collision shape ignores its transform matrix
	private:
		glm::vec3 min_point;
		glm::vec3 max_point;
	
	public :
		CollisionAAB(glm::vec3 min_point, glm::vec3 max_point);
		glm::vec3 support(glm::vec3 dir) override;
		void get_faces(std::vector<glm::vec4> *faces) override;
};

class CollisionCube: public ColSharpShape {
	private:
		float size;	// the length of an edge

	public:
		CollisionCube(float size);
		glm::vec3 support(glm::vec3 dir) override;
		void get_faces(std::vector<glm::vec4> *faces) override;
};


// Smooth Shapes ===============================================================================================

class ColSmoothShape: public ColShape {
	public:
		bool is_colliding(ColShape *other);
		glm::vec4 resolve_collision(ColSharpShape *other);
		glm::vec4 resolve_collision(ColSmoothShape *other);
};

class CollisionSphere: public ColSmoothShape {
	private:
		float radius;

	public:
		CollisionSphere(float radius);
		glm::vec3 support(glm::vec3 dir) override;
};

class CollisionCapsule: public ColSmoothShape {
	private:
		float radius;
		float height;	// hemisphere center to hemisphere center
	
	public:
		CollisionCapsule(float radius, float height);
		glm::vec3 support(glm::vec3 dir) override;
};

#endif //COLLISION_SHAPES_HDD