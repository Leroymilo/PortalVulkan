#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct Texture {
	VkImage texture_image;
	VkDeviceMemory texture_image_memory;
	VkImageView texture_image_view;
	VkSampler texture_sampler;

	void cleanup(VkDevice &device);
};

#endif //TEXTURE_HPP