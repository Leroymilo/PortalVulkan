#ifndef COLLISION_SHAPES_HDD
#define COLLISION_SHAPES_HDD

#include "glm/glm.hpp"


struct Edge {
	size_t a;
	size_t b;

	bool operator==(const Edge &other) const;

	struct hash {
		size_t operator()(const Edge &edge) const;
	};
};

class ColShape {
	protected:
		glm::mat4 matrix = glm::mat4(1.0f);
	
	public:
		void set_transform(glm::mat4 new_matrix);
		virtual glm::vec3 support_function(glm::vec3 dir) = 0;
		bool GJK(ColShape *other, std::vector<glm::vec3> *simplex);
		glm::vec3 EPA(ColShape *other, std::vector<glm::vec3> &vertices);
};

class CollisionSphere: public ColShape {
	private:
		float radius;

	public:
		CollisionSphere(float radius);
		glm::vec3 support_function(glm::vec3 dir) override;
};

class CollisionAAB: public ColShape {
	private:
		glm::vec3 min_point;
		glm::vec3 max_point;
	
	public :
		CollisionAAB(glm::vec3 min_point, glm::vec3 max_point);
		glm::vec3 support_function(glm::vec3 dir) override;
};

#endif //COLLISION_SHAPES_HDD