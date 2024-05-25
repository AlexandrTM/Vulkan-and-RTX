#include "pch.h"
#include "VulkanInitializer.h"
#include "InputHandler.h"
#include "Vertex.h"
#include "Model.h"

#ifndef VULKAN_AND_RTX_H
#define VULKAN_AND_RTX_H

class VulkanAndRTX {
private:
#pragma region

	uint32_t windowWidth = 800;
	uint32_t windowHeight = 450;
	const int MAX_FRAMES_IN_FLIGHT = 2;

	const std::string MODEL_PATH = "models/model.obj";
	const std::string TEXTURE_PATH = "textures/texture.png";

#pragma endregion
#pragma region

	struct Models {
		std::vector<Model> objects;
		Model sky;
	} models;

	// MVP matrix and other info for shaders
	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;
		alignas(4)  glm::vec3 sun;
		alignas(4)  glm::vec3 viewer;
	} objectUBO, skyUBO;

	GLFWwindow* window;

	InputHandler inputHandler;
	VulkanInitializer vkInit;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkRenderPass renderPass;
	VkPipelineLayout objectPipelineLayout;
	VkPipeline		 objectPipeline;
	VkPipelineLayout skyPipelineLayout;
	VkPipeline		 skyPipeline;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	uint32_t currentFrame = 0;

	bool framebufferResized = false;

	std::vector<VkBuffer>       objectUniformBuffers;
	std::vector<VkDeviceMemory> objectUniformBuffersMemory;
	std::vector<VkBuffer>       skyUniformBuffers;
	std::vector<VkDeviceMemory> skyUniformBuffersMemory;

	VkDescriptorPool             descriptorPool;
	VkDescriptorSetLayout        descriptorSetLayout;

	struct DescriptorSets {
		VkDescriptorSet objects;
		VkDescriptorSet sky;
	};
	std::vector<DescriptorSets> descriptorSets;

	VkImage		   textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView    textureImageView;
	VkSampler	   textureSampler;

	VkImage        depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView    depthImageView;

	VkImage        colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView    colorImageView;

	uint32_t mipLevels;
#pragma endregion

public:
	void run();

private:
	// initializing GLFW and creating window
	void createWindow();

	void prepareResources();

	void mainLoop();

	void cleanupModels();
	// emptying RAM
	void cleanupMemory();

	// cleaning "out of date" swap chain
	void cleanupSwapChain();

	// recreating swap chain in some special cases
	void recreateSwapChain();

	// set "framebufferResized" to "true" if window was resized or moved
	void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	void generateCubicLandscape(size_t landscapeWidth, size_t landscapeLenght, float_t cubeSize);

	void generateCube(float x, float y, float z, float_t cubeSize);

	void loadModel(const std::string& filePath);
	void loadGltfModel(const std::string& filePath);
	// reading bytecode files and returning its bytes
	static std::vector<char> readFile(const std::string& filename);

	void createTextureImage(std::string texturePath, VkImage& textureImage,
		VkDeviceMemory& textureImageMemory);

	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	// creating image for MSAA sampling
	void createColorResources();

	// three objects for depth testing
	void createDepthResources();

	// check if the format has stencil component
	bool hasStencilComponent(VkFormat format);

	// finding most appropriate format for the depth test
	VkFormat findDepthFormat();

	// finding most desirable format of color for a given situation
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
		VkFormatFeatureFlags features);

	// how to sample through texels of the texture for drawing them on 3D model
	void createTextureSampler(VkSampler& vkSampler);

	void createTextureImageView(VkImage& textureImage, VkImageView& textureImageView);

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	// transitioning image to the right layout
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
		VkImageLayout newLayout, uint32_t mipLevels);

	// allocating and beginning command buffer helper function
	VkCommandBuffer beginSingleTimeCommands();

	// ending and submitting command buffer helper function
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VkDeviceMemory& imageMemory);

	// finding most appropriate memory type depending on buffer and application properties
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	// copying contents of one buffer to another
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void createVertexBuffer(Model& model);

	void createIndexBuffer(Model& model);

	void createDescriptorSets();

	// create multiple command buffers
	void createCommandBuffers();

	// record commands to the command buffer
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	// creating uniform buffer for each frame in flight
	void createUniformBuffers();

	void createDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout) const;

	void createDescriptorPool();

	// creating swap chain with the best properties for current device
	void createSwapChain();

	void createSwapChainImageViews();

	// information about framebuffer attachments, how many color and depth buffers there will
	// be, how many samples to use for each of them and how their contents should be treated
	void createRenderPass();

	void createPipelineLayout(VkDescriptorSetLayout& descriptorSetLayout, 
		VkPipelineLayout& pipelineLayout) const;

	// transfering scene to images
	void createGraphicsPipeline(const std::string& vertexShader,
		const std::string& fragmentShader, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout);

	// creating framebuffer from each swap chain image view
	void createSwapChainFramebuffers();

	// for specific queue family
	void createCommandPool();

	// Creating fences and semaphores
	void createSyncObjects();

	// Creating frames for presentation
	void drawFrame(float timeSinceLaunch);

	// updating MVP matrix for every draw call every frame
	void updateUniformBuffers(uint32_t currentImage, float timeSinceLaunch);

	// wraping shader
	VkShaderModule createShaderModule(const std::vector<char>& code) const;

	// choosing best surface format(color space and number of bits) for the swap chain
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	// chosing best present mode to window surface
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	// chosing best swap chain extent(resolution of the images)
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};

#endif // !VULKAN_AND_RTX_H
