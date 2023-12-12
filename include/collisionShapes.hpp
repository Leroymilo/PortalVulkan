#ifndef COLLISION_SHAPES_HDD
#define COLLISION_SHAPES_HDD

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ColShape {
	protected:
		glm::mat4 matrix = glm::mat4(1.0f);
	
	public:
		void set_transform(glm::mat4 new_matrix);
		virtual glm::vec4 support_function(glm::vec4 dir) = 0;
		bool collides(ColShape *other);
};

class CollisionSphere: public ColShape {
	private:
		float radius;

	public:
		CollisionSphere(float radius);
		glm::vec4 support_function(glm::vec4 dir);
};

class CollisionAAB: public ColShape {
	private:
		glm::vec4 min_point;
		glm::vec4 max_point;
	
	public :
		CollisionAAB(glm::vec4 min_point, glm::vec4 max_point);
		glm::vec4 support_function(glm::vec4 dir);
};

#endif //COLLISION_SHAPES_HDD