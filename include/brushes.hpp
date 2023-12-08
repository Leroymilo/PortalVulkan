#ifndef BRUSHES_HPP
#define BRUSHES_HPP

#include <glm/glm.hpp>

class SimpleBrush {
	private:
		glm::vec3 min_point = glm::vec3();
		glm::vec3 max_point = glm::vec3();

		int generate_buffers();
	
	public:
		SimpleBrush(glm::vec3 min_point, glm::vec3 max_point);
};

#endif //BRUSHES_HPP