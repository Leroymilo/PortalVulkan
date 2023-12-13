#ifndef COLLISION_SHAPES_HDD
#define COLLISION_SHAPES_HDD

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ColShape {
	protected:
	
	public:
		glm::mat4 matrix = glm::mat4(1.0f);
		
		void set_transform(glm::mat4 new_matrix);
		virtual glm::vec3 support_function(glm::vec3 dir) = 0;
		bool collides(ColShape *other);
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