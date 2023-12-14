#ifndef VULKAN_APP_HPP
#define VULKAN_APP_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <optional>
#include <vector>
#include <unordered_map>

#include "texture.hpp"
#include "world.hpp"
#include "vertex.hpp"


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete();
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

static std::vector<char> readFile(const std::string& filename);

static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
);

VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger
);

void DestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator
);

class VulkanApp {
	private:
		GLFWwindow* window;

		World world;

		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;	// logical device
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
		
		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		std::vector<VkImageView> swapChainImageViews;

		VkRenderPass renderPass;
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;

		std::vector<VkFramebuffer> swapChainFramebuffers;

		VkCommandPool commandPool;
		
		std::unordered_map<std::string, Texture> textures;
		std::unordered_map<std::string, Texture> norm_maps;

		VkImage colorImage;
		VkDeviceMemory colorImageMemory;
		VkImageView colorImageView;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;

		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;

		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		uint32_t currentFrame = 0;

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		#ifdef NDEBUG
			const bool enableValidationLayers = false;
		#else
			const bool enableValidationLayers = true;
		#endif

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// ========= Window setup =========
		void initWindow();

		// ========= Vulkan setup =========
		bool checkValidationLayerSupport();
		std::vector<const char*> getRequiredExtensions();

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		void createInstance();

		void setupDebugMessenger();

		void createSurface();

		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice &ph_device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice &ph_device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice &ph_device);
		VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice &physical_device);
		int isDeviceSuitable(VkPhysicalDevice ph_device);	// 0 means unsuitable, any number > 0 is a score
		void pickPhysicalDevice();

		void createLogicalDevice();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void createSwapChain();

		VkImageView createImageView(
			VkImage image, VkFormat format,
			VkImageAspectFlags aspectFlags,
			uint32_t nb_mip_levels
		);

		void createImageViews();

		void createRenderPass();

		void createDescriptorSetLayout();

		VkShaderModule createShaderModule(const std::vector<char>& code);
		void createGraphicsPipeline();

		void createFramebuffers();

		void createCommandPool();

		void createColorResources();

		VkFormat findSupportedFormat(
			const std::vector<VkFormat>& candidates,
			VkImageTiling tiling, VkFormatFeatureFlags features
		);
		VkFormat findDepthFormat();
		void createDepthResources();

		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void createBuffer(
			VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
			VkBuffer& buffer, VkDeviceMemory& bufferMemory
		);

		void createImage(
			uint32_t width, uint32_t height, uint32_t nb_mip_levels,
			VkSampleCountFlagBits numSamples,
			VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
			VkImage& image, VkDeviceMemory& imageMemory
		);
		void transitionImageLayout(
			VkImage image, VkFormat format, uint32_t nb_mip_levels,
			VkImageLayout oldLayout, VkImageLayout newLayout
		);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void generateMipmaps(
			VkImage image, VkFormat imageFormat,
			int32_t texWidth, int32_t texHeight, uint32_t nb_mip_levels
		);
		Texture createTexture(std::string tex_name);
		void createTextures();

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    	void createVertexBuffer();

		void createIndexBuffer();

		void createUniformBuffers();

		void createDescriptorPool();

    	void createDescriptorSets();

		void createCommandBuffer();

		void createSyncObjects();

		void initVulkan();

		void updateUniformBuffer(uint32_t currentImage);
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void drawFrame();

		// ========= Main loop =========
		void recreateSwapChain();
		void mainLoop();

		// ========= Cleanup =========
		void cleanupSwapChain();
		void cleanup();

	public:
		bool framebufferResized = false;

		void run();
};

#endif