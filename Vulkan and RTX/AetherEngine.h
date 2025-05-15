#include "pch.h"
	
#ifndef VULKAN_AND_RTX_H
#define VULKAN_AND_RTX_H

#include "VulkanInitializer.h"

#include "UserInterfaceElement.h"

#include "InGameWindow.h"
#include "MainWindow.h"

#include "MainMenuSlotHandler.h"
#include "SettingsMenuSlotHandler.h"
#include "PauseMenuSlotHandler.h"
#include "SelectEquationSlotHandler.h"
#include "SolveEquationSlotHandler.h"

#include "ModelManager.h"
#include "gamecontext_instance.h"
#include "Model.h"
#include "TerrainGenerator.h"
#include "Character.h"
#include "Vertex.h"
#include "DungeonComponents.h"
#include "Dungeon.h"
#include "Equations.h"

class AetherEngine : public QObject {
	Q_OBJECT

private:
#pragma region
	VulkanInitializer vkInit;
	VmaAllocator vmaAllocator;

	float gravity = 9.81f;
	Character character;

	uint32_t windowWidth = 0;
	uint32_t windowHeight = 0;
	double lastMousePosX, lastMousePosY;

	MainWindow* mainWindow = nullptr;
	QStackedWidget* stackedWidget = nullptr;

	InGameWindow* inGameWindow = nullptr;
	QWidget* inGameWidget = nullptr;

	QVulkanInstance qVulkanInstance;

	VkDescriptorPool          descriptorPool;
	VkDescriptorSetLayout     descriptorSetLayout;

	UniformBufferObject		  skyUBO;
	UniformBufferObject		  meshUBO;
	ShaderStorageBufferObject boneSSBO;

	std::unique_ptr<TerrainGenerator> terrainGenerator;

	int32_t puzzleInput = 0;
	bool puzzleGenerated = false;
	int32_t puzzleAnswer = 0;
	std::string puzzleEquation;
	float timeToSolvePuzzle = 0.0f;

	VkSwapchainKHR			   swapchain;
	std::vector<VkImage>	   swapchainImages;
	VkFormat				   swapchainImageFormat;
	VkExtent2D				   swapchainExtent;
	std::vector<VkImageView>   swapchainImageViews;
	std::vector<VkFramebuffer> swapchainFramebuffers;

	VkRenderPass objectRenderPass;
	VkRenderPass uiRenderPass;
	VkPipelineLayout pipelineLayout;

	std::unordered_map<std::string, VkPipeline> pipelines;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence>     inFlightFences;
	uint32_t currentFrame = 0;

	std::unordered_map<uiElementId, UserInterfaceElement> uiMap;

	bool isSelectEquationActivated = false;
	bool isSolveEquationTextFieldActivated = false;

	bool isFramebufferResized = false;

	std::vector<Model> models;
	Model			   sky;

	std::unordered_map<std::string, Texture> dungeonTextures;
	mutable std::unordered_set<uint64_t> deletedTextureHashes;

	Texture grassTexture;
	Texture floor_background;
	Texture	transparentTexture;
	Texture	notFoundTexture;

	Texture depthTexture;
	Texture msaaTexture;


	size_t createdBuffers = 0;
	mutable size_t destroyedVmaAllocations = 0;

private slots:
	void onMainWindowResized(int width, int height);
	void onMainWindowMoved(int x, int y);
	void onInGameWindowMoved(int x, int y);
	void onInGameWindowLostFocus();

#pragma endregion

public:
	void run();

private:
	void changeState(GameState newGameState);
	//void handleDungeonExplorationState(double deltaTime, double timeSinceLaunch);
	void handleInGameTestingState(double deltaTime, double timeSinceLaunch);

	void updateSelectEquation(size_t amountOfEquations);
	void updateInGameOverlay();
	void updateSolveEquationOverlay();
	void clearSolveEquationInput();

	void handleEquationSolving();
	void handleRightAnswer(Equation& selectedEquation);
	void handleWrongAnswer(const Equation& selectedEquation);

	std::string createPuzzleEquation(std::string name, int32_t& answer);

	void setWindowSize();

	UserInterfaceElement createUIElement(
		const QString& qmlPath, size_t windowWidth, size_t windowHeight, QObject* parent
	);
	void changeUIElementSize(
		UserInterfaceElement& uiElement, size_t windowWidth, size_t windowHeight
	);
	void createUIElements();

	void createMainWindow();
	void createInGameWindow();

	void prepareResources();
	void prepareUI();
	void recordUiElementToCommandBuffer(UserInterfaceElement& uiElement, VkCommandBuffer commandBuffer);
	void renderQmlToTexture(UserInterfaceRenderer* userInterfaceRenderer, Texture& texture);

	void mainLoop();

	void recreateDungeonFloor(int32_t floorNumber, float difficultyScale);
	void cleanupModels(std::vector<Model>& models) const;
	void cleanupModel(Model& model) const;
	void cleanupMesh(Mesh& mesh) const; 
	void cleanupShaderBuffers(std::vector<Model>& models) const;
	void cleanupShaderBuffers(Model& model) const;
	void cleanupShaderBuffers(Mesh& mesh) const;
	void cleanupTexture(Texture& texture) const;
	void cleanupMaterial(Material& material) const;
	void cleanupSwapchain();

	void restrictCharacterMovement(Camera& camera);

	void cleanupMemory();

	// recreating swap chain in some special cases
	void recreateSwapchain();

	void loadModelsFromFolder(
		const std::string& directory,
		std::vector<Model>& models
	);
	Texture loadTextureForModel(const std::string& texturePath, const aiScene* scene);
	void loadTexturesFromFolder(
		const std::string& texturePath, 
		std::unordered_map<std::string, Texture>& textures
	);
	Material processMaterial(aiMaterial* aiMat, const aiScene* scene);
	void processNode(
		aiNode* node, const aiScene* scene,
		std::vector<Model>& models,
		Model& parentModel,
		glm::mat4 parentTransform, const glm::mat4 globalInverseTransform,
		uint32_t& perModelVertexOffset,
		int level
	);

	void uploadRawDataToTexture(void* rawImage, uint32_t width, uint32_t height, Texture& texture);
	void createSolidColorTexture(
		std::array<uint8_t, 4> color, uint32_t width, uint32_t height, Texture& texture
	);
	void loadTextureFromPath(const std::string& texturePath, Texture& texture);
	void createTextureFromEmbedded(
		const std::string& embeddedTextureName, 
		const aiScene* scene, Texture& texture
	);
	void createImage(
		uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VmaAllocation& vmaAllocation
	);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	QColor interpolateColor(const QColor& from, const QColor& to, double t);

	// how to sample through texels of the texture for drawing them on 3D model
	void createTextureSampler(uint32_t& mipLevels, VkSampler& textureSampler) const;
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) const;
	void transitionImageLayout(
		VkImage image, VkFormat format, VkImageAspectFlags aspectMask,
		VkImageLayout oldLayout, VkImageLayout newLayout,
		uint32_t mipLevels
	);

	// creating image for MSAA sampling
	void createColorTexture(Texture& texture);
	void createDepthTexture(Texture& texture);

	// check if the format has stencil component
	bool hasStencilComponent(VkFormat format);

	VkFormat findDepthFormat() const;
	// finding most desirable format of color for a given situation
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
		VkFormatFeatureFlags features) const;

	// allocating and beginning command buffer helper function
	VkCommandBuffer beginSingleTimeCommands() const;
	// ending and submitting command buffer helper function
	void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

	// finding most appropriate memory type depending on buffer and application properties
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	// copying contents of one buffer to another
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void createBuffer(
		VkDeviceSize size, VkBufferUsageFlags usage, 
		VkMemoryPropertyFlags properties, bool isMappingRequired,
		VkBuffer& buffer, VmaAllocation& vmaAllocation
	);

	void createVertexBuffer(Mesh& mesh);
	void createIndexBuffer(Mesh& mesh);

	void computeAABB_createVertexIndexBuffers(std::vector<Model>& models);
	void computeAABB_createVertexIndexBuffers(Model& model);
	void computeAABB_createVertexIndexBuffers(Mesh& mesh);

	void createShaderBuffers(std::vector<Model>& models, size_t swapchainImageCount);
	void createShaderBuffers(Model& model, size_t swapchainImageCount);
	void createShaderBuffers(Mesh& mesh, size_t swapchainImageCount);

	void createDescriptorSets(std::vector<Model>& models, size_t swapchainImageCount);
	void createDescriptorSets(Model& model, size_t swapchainImageCount);
	void createDescriptorSets(Mesh& mesh, size_t swapchainImageCount);

	void createDescriptorPool(size_t modelsNum, size_t meshesNum, VkDescriptorPool& pDescriptorPool);
	void createDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout) const;
	void createDescriptorSet(VkDescriptorSet& descriptorSet);
	void addTextureToDescriptorSet(
		const Texture& texture,
		VkDescriptorSet descriptorSet, uint32_t dstBinding
	) const;
	void addTextureIfExistToDescriptorSet(
		const Texture& texture,
		VkDescriptorSet descriptorSet, uint32_t dstBinding
	) const;
	void addBufferToDescriptorSet(
		VkDescriptorSet descriptorSet, VkBuffer buffer,
		VkDescriptorType descriptorType,
		size_t bufferSize, uint32_t dstBinding
	) const;

	// void bindVertexAndIndexBuffersToCommandBuffer(const Model& model, VkCommandBuffer commandBuffer);
	void bindVertexAndIndexBuffersToCommandBuffer(const Mesh& mesh, VkCommandBuffer commandBuffer);

	void updateShaderBuffers(uint32_t currentImage, double timeSinceLaunch);
	// Creating frames for presentation
	void drawFrame(double timeSinceLaunch, double deltaTime/*, ImDrawData* draw_data*/);

	void initVMA();

	// create multiple command buffers
	void createCommandBuffers();
	// record commands to the command buffer
	void recordCommandBuffer(
		VkCommandBuffer commandBuffer, uint32_t imageIndex,
		double timeSinceLaunch, double deltaTime
		/*,ImDrawData* draw_data*/
	);
	void recordModelsToCommandBuffer(const std::vector<Model>& models, VkCommandBuffer commandBuffer);
	void recordModelToCommandBuffer(const Model& model, VkCommandBuffer commandBuffer);

	// creating swap chain with the best properties for current device
	void createSwapchain();
	void createSwapchainImageViews();
	// creating framebuffer from each swap chain image view
	void createSwapchainFramebuffers();
	// choosing best present mode for window surface
	VkPresentModeKHR chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	// choosing best swap chain extent(resolution of the images)
	VkExtent2D chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	// choosing best surface format(color space and number of bits) for the swap chain
	VkSurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	// information about framebuffer attachments, how many color and depth buffers there will
	// be, how many samples to use for each of them and how their contents should be treated
	void createObjectRenderPass(VkRenderPass& renderPass) const;
	void createUiRenderPass(VkRenderPass& renderPass) const;

	void createPipelineLayout(
		VkDescriptorSetLayout& descriptorSetLayout, 
		VkPipelineLayout& pipelineLayout
	) const;
	void createGraphicsPipeline(
		const PipelineType pipelineType, const std::string pipelineName,
		const std::string& vertexShader, const std::string& fragmentShader,
		const VkRenderPass& renderPass
	);
	void createPipelinesAndSwapchain();

	// for specific queue family
	void createCommandPool();

	// Creating fences and semaphores
	void createSyncObjects();

	// reading byte code files and returning its bytes
	static std::vector<char> readFile(const std::string& filename);
	// wrapping shader
	VkShaderModule createShaderModule(const std::vector<char>& code) const;
};

#endif // !VULKAN_AND_RTX_H
