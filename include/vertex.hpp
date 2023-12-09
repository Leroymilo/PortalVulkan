#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <array>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/glm.hpp>

#ifndef offsetof
// because VSCode can't find it for some reason...
#define offsetof(s, m) (long)(&(((s*)0)->m))
#endif

struct Vertex {
    glm::vec3 pos;
	glm::vec2 texCoord;
    glm::vec3 normal;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
	bool operator==(const Vertex& other) const;
};

template<> struct std::hash<Vertex> {
	std::size_t operator()(Vertex const& vertex) const {
		std::size_t result = std::hash<glm::vec3>()(vertex.pos);
		result = (result ^ (std::hash<glm::vec2>()(vertex.texCoord) << 1)) >> 1;
		result = result ^ (std::hash<glm::vec3>()(vertex.normal) << 1);
		return result;
	}
};

#endif //VERTEX_HPP