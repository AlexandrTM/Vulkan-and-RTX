#include "pch.h"
#include "VulkanInitializer.h"
#include "InputHandler.h"
#include "Vertex.h"
#include "Model.h"
#include "TerrainGenerator.h"

#ifndef VULKAN_AND_RTX_H
#define VULKAN_AND_RTX_H

static const uint32_t BONES_NUM = 256;

class VulkanAndRTX {
private:
#pragma region

	uint32_t windowWidth = 800;
	uint32_t windowHeight = 450;
	const int MAX_FRAMES_IN_FLIGHT = 2;

	const std::string MODEL_PATH = "models/model.obj";

#pragma endregion
#pragma region

	VkDescriptorPool            descriptorPool;
	VkDescriptorSetLayout       descriptorSetLayout;

	// MVP matrix and other info for shaders
	struct UniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 projection;
		alignas(16) glm::vec3 sun;
		alignas(16) glm::vec3 observer;
		alignas(4) float visibilityRange = 6000;
	};

	struct BoneUniformBufferObject {
		alignas(16) std::vector<glm::mat4> boneTransforms;

		BoneUniformBufferObject() {
			boneTransforms.resize(BONES_NUM);
			for (size_t i = 0; i < BONES_NUM; ++i) {
				boneTransforms[i] = glm::mat4(0.0f); // Initialize each matrix to zero
			}
		}
	};

	UniformBufferObject									   skyUBO;
	std::vector<VkBuffer>								   skyUniformBuffers;
	std::vector<VkDeviceMemory>							   skyUniformBuffersMemory;
	std::vector<VkDescriptorSet>						   skyDescriptorSets;

	UniformBufferObject									   meshUBO;
	std::vector<std::vector<std::vector<VkBuffer>>>        meshUniformBuffers; // For all models, meshes, and frames
	std::vector<std::vector<std::vector<VkDeviceMemory>>>  meshUniformBuffersMemory;
	std::vector<std::vector<std::vector<VkDescriptorSet>>> meshDescriptorSets;

	BoneUniformBufferObject								   boneUBO;
	std::vector<std::vector<VkBuffer>>					   boneUniformBuffers; // For all models and frames
	std::vector<std::vector<VkDeviceMemory>>			   boneUniformBuffersMemory;
	std::vector<std::vector<VkDescriptorSet>>			   boneDescriptorSets;

	std::unique_ptr<TerrainGenerator> terrainGenerator;

	GLFWwindow* window;
	ImGui_ImplVulkanH_Window* vulkanWindow;
	double lastMousePosX, lastMousePosY;

	ImGuiIO io;
	int puzzleInput = 0;
	bool puzzleGenerated = false;
	int puzzleAnswer = 0;
	std::string puzzleEquation;
	float timeToSolvePuzzle = 0.0f;

	InputHandler inputHandler;
	VulkanInitializer vkInit;

	VkSwapchainKHR			   swapChain;
	std::vector<VkImage>	   swapChainImages;
	VkFormat				   swapChainImageFormat;
	VkExtent2D				   swapChainExtent;
	std::vector<VkImageView>   swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkRenderPass objectRenderPass;
	VkPipelineLayout pipelineLayout;

	std::unordered_map<std::string, VkPipeline> pipelines;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	uint32_t currentFrame = 0;

	bool framebufferResized = false;

	std::vector<Model> models;
	Model sky;

	VkSampler	   textureSampler;

	Texture        texture;

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

	void setupImguiWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, 
		size_t width, size_t height);
	void setupImGui();
	void cleanupImGui();
	void check_vk_result(VkResult err);
	std::string createPuzzleEquation(std::string name, int& answer);

	void prepareResources();

	void mainLoop();

	void restrictCharacterMovement(Camera& camera);

	void cleanupModels();
	void cleanupModel(Model& model) const;
	// emptying RAM
	void cleanupMemory();

	// cleaning "out of date" swap chain
	void cleanupSwapChain();

	// recreating swap chain in some special cases
	void recreateSwapChain();

	// set "framebufferResized" to "true" if window was resized or moved
	void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	void generateCubicLandscape(size_t landscapeWidth, size_t landscapeLenght, float_t cubeSize);
	void generateCube(float x, float y, float z, float cubeSize);
	void generateCuboid(float x, float y, float z,
		float width, float height, float length, glm::vec3 color);
	void createSkyCube();
	void generateTerrain(float startX, float startZ, size_t width, size_t length,
		float gridSize, float scale, float roughness, size_t seed);

	void loadObjModel(const std::string& filePath);
	void loadGltfModel(const std::string& filePath);

	void loadModelsFromDirectory(const std::string& directory, std::vector<Model>* models);
	Texture loadTexture(const std::string& texturePath, const aiScene* scene);
	Material processMaterial(aiMaterial* aiMat, const aiScene* scene);
	void processNode(
		aiNode* node, const aiScene* scene,
		std::vector<Model>* models,
		Model& parentModel, glm::mat4 parentTransform,
		std::unordered_map<std::string, size_t>& boneMap,
		uint32_t& perModelVertexOffset,
		int level
	);

	void createTextureFromPath(const std::string& texturePath, Texture& texture);
	void createTextureFromEmbedded(const std::string& embeddedTextureName, Texture& texture, const aiScene* scene);
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VkDeviceMemory& imageMemory);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	// how to sample through texels of the texture for drawing them on 3D model
	void createTextureSampler(VkSampler& vkSampler);
	void createTextureImageView(VkImage& textureImage, VkImageView& textureImageView);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	// transitioning image to the right layout
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
		VkImageLayout newLayout, uint32_t mipLevels);

	// reading bytecode files and returning its bytes
	static std::vector<char> readFile(const std::string& filename);

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

	// allocating and beginning command buffer helper function
	VkCommandBuffer beginSingleTimeCommands() const;

	// ending and submitting command buffer helper function
	void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

	// finding most appropriate memory type depending on buffer and application properties
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	// copying contents of one buffer to another
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void createVertexBuffer(Model& model);
	void createIndexBuffer(Model& model);

	void createSkyUniformBuffers(size_t swapChainImageCount);
	void createMeshShaderBuffers(size_t swapChainImageCount);
	void createSkyDescriptorSets(size_t swapChainImageCount);
	void createMeshDescriptorSets(size_t swapChainImageCount);

	void createDescriptorPool();
	void createDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout) const;
	void createDescriptorSet(
		VkDescriptorSet& descriptorSet,
		VkBuffer buffer, VkDescriptorType descriptorType,
		size_t bufferSize, uint32_t dstBinding
	);
	void addTextureToDescriptorSet(
		const Texture& texture,
		VkDescriptorSet descriptorSet, uint32_t dstBinding
	) const;
	void addBufferToDescriptorSet(
		VkDescriptorSet descriptorSet, VkBuffer buffer,
		VkDescriptorType descriptorType,
		size_t bufferSize, uint32_t dstBinding
	) const;

	void drawModel(VkCommandBuffer commandBuffer, const Model& model);
	void drawMesh(VkCommandBuffer commandBuffer, const Mesh& mesh);

	// updating MVP matrix for every draw call every frame
	void updateUniformBuffers(uint32_t currentImage, float timeSinceLaunch);

	// create multiple command buffers
	void createCommandBuffers();
	// record commands to the command buffer
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, ImDrawData* draw_data);

	// creating swap chain with the best properties for current device
	void createSwapChain();
	void createSwapChainImageViews();
	// creating framebuffer from each swap chain image view
	void createSwapChainFramebuffers();
	// chosing best present mode to window surface
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	// chosing best swap chain extent(resolution of the images)
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	// information about framebuffer attachments, how many color and depth buffers there will
	// be, how many samples to use for each of them and how their contents should be treated
	void createRenderPass(VkRenderPass& renderPass);

	void createPipelineLayout(VkDescriptorSetLayout& descriptorSetLayout);
	void createGraphicsPipeline(
		const std::string prefix, 
		const std::string& vertexShader, const std::string& fragmentShader
	);

	// for specific queue family
	void createCommandPool();

	// Creating fences and semaphores
	void createSyncObjects();

	// Creating frames for presentation
	void drawFrame(float timeSinceLaunch, ImDrawData* draw_data);

	// wraping shader
	VkShaderModule createShaderModule(const std::vector<char>& code) const;

	// choosing best surface format(color space and number of bits) for the swap chain
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
};

#endif // !VULKAN_AND_RTX_H
