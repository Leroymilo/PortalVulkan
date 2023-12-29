#ifndef RENDER_INFO_HPP
#define RENDER_INFO_HPP

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/glm.hpp>

struct RenderInfo {
	VkCommandBuffer command_buffer;
	VkPipelineLayout pipeline_layout;
	std::vector<VkDescriptorSet> descriptor_sets;
	uint32_t current_frame;
	uint32_t dynamicAlignment;
};

#endif //RENDER_INFO_HPP