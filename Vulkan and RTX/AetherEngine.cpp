#include "pch.h"
#include "AetherEngine.h"

void AetherEngine::run()
{
	// createGLFWWindow();
	// character.initializeInputHandler();
	createInGameWindow();
	createMainMenuWidget();
	createMainWindow();

	vkInit.initializeVulkan(&qVulkanInstance);
	prepareResources();
	mainLoop();
	cleanupMemory();
}

//void AetherEngine::createGLFWWindow()
//{
//	glfwInit();
//
//	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
//	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
//
//	glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "Vulkan and RTX", nullptr, nullptr);
//	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
//	int count;
//	GLFWmonitor** monitors = glfwGetMonitors(&count);
//	std::cout << "monitor count: " << count << "\n";
//	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
//	std::cout << "current monitor mode: " << mode->redBits << " " << mode->greenBits << " " << mode->blueBits
//		<< " " << mode->width << " " << mode->height << " " << mode->refreshRate << "\n";
//
//	// setting glfwWindow to the center
//	glfwSetWindowPos(glfwWindow, mode->width / 2 - windowWidth / 2, mode->height / 2 - windowHeight / 2 - 30);
//
//	/*int width_mm, height_mm;
//	glfwGetMonitorPhysicalSize(monitor, &width_mm, &height_mm);
//	std::cout << "monitor width, height: " << width_mm << " " << height_mm << "\n";
//	const char* name = glfwGetMonitorName(monitor);
//	std::cout << "monitor name: " << name << "\n";
//
//	int count1;
//	const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count1);
//
//	for (int i = 0; i < count1; i++)
//	{
//		std::cout << "video mode: " << i << " " << modes[i].redBits << " " << modes[i].greenBits << " " << modes[i].blueBits << " "
//			<< modes[i].width << " " << modes[i].height << " " << modes[i].refreshRate << "\n";
//	}*/
//
//	// Icon creation
//#pragma region
//	int width, height;
//	int channels;
//	unsigned char* pixels = stbi_load("textures/icon.png", &width, &height, &channels, 4);
//
//	GLFWimage windowIcon[1]{};
//	windowIcon[0].height = height;
//	windowIcon[0].width = width;
//	windowIcon[0].pixels = pixels;
//
//	glfwSetWindowIcon(glfwWindow, 1, windowIcon);
//#pragma endregion
//}
void AetherEngine::createInGameWindow()
{
	qVulkanInstance.setApiVersion(QVersionNumber(1, 0));
	/*auto supportedExtensions = qVulkanInstance.extensions();
	qDebug() << "Supported extensions:" << supportedExtensions;*/

	if (vkInit.enableValidationLayers) {
		qVulkanInstance.setLayers(QByteArrayList() << "VK_LAYER_KHRONOS_validation");
		qVulkanInstance.clearDebugOutputFilters();
	}

	if (!qVulkanInstance.create()) {
		throw std::runtime_error("Failed to create Vulkan instance in Qt.");
	}

	QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
	windowWidth = screenGeometry.width() / 1.7;
	windowHeight = screenGeometry.height() / 1.7;

	InGameWindow* qtWindow = new InGameWindow(&qVulkanInstance, character, gameContext);
	qtWindow->resize(windowWidth, windowHeight);
	qtWindow->setTitle("Vulkan and RTX");
	qtWindow->create();

	vkInit.surface = qVulkanInstance.surfaceForWindow(qtWindow);
	if (vkInit.surface == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to create Vulkan surface for Qt window.");
	}

	this->inGameWindow = qtWindow;
	inGameWidget = QWidget::createWindowContainer(inGameWindow);

	connect(qtWindow, &InGameWindow::framebufferResized, this, &AetherEngine::onFramebufferResized);
}
void AetherEngine::createMainMenuWidget() {
	mainMenuWidget = new MainMenuWidget();
	mainMenuWidget->resize(windowWidth, windowHeight);

	connect(mainMenuWidget, &MainMenuWidget::startGame, [this]() {
		gameContext.requestedGameState = GameState::IN_GAME;
		});

	connect(mainMenuWidget, &MainMenuWidget::openSettings, [this]() {
		gameContext.requestedGameState = GameState::SETTINGS;
		});

	connect(mainMenuWidget, &MainMenuWidget::exitGame, [this]() {
		gameContext.requestedGameState = GameState::EXIT;
		});
}
void AetherEngine::createMainWindow()
{
	mainWindow = new MainWindow(mainMenuWidget, inGameWidget);
	mainWindow->resize(windowWidth, windowHeight);
	mainWindow->show();

	stackedWidget = mainWindow->getStackedWidget();

	gameContext.requestedGameState = GameState::IN_GAME;
}

void AetherEngine::onFramebufferResized(int width, int height) {
	//std::cout << "onFramebufferResized: " << width << " " << height << "\n";
    isFramebufferResized = true;
    character.camera.setViewportSize(width, height);
}

void AetherEngine::prepareResources()
{
	//std::chrono::high_resolution_clock::time_point currentTime;
	//float deltaTime;
	//std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
	
	/*currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - previousTime).count();
	if (deltaTime > 0.00001) {
		std::cout << "time to create gpp: " << deltaTime << "\n";
	}*/
	
	createDescriptorSetLayout(descriptorSetLayout);
	createPipelinesAndSwapchain();
	createCommandPool();
	createColorTexture(msaaTexture);
	createDepthTexture(depthTexture);
	createSwapchainFramebuffers();
	createCommandBuffers();
	createSyncObjects();

	createTextureFromPath("textures/grass001.png", grassTexture);
	createDummyTexture({ 0, 0, 0, 255 }, dummyTexture);
	
	TerrainData terrainData = {
		100, 100, // chunkWidth, chunkLength
		4, 4,     // chunkRows, chunkCols
		2.0f,     // gridSize
		0.1f,     // scale
		2.0f,     // height
	};
	terrainGenerator = std::make_unique<TerrainGenerator>(1);
	TerrainGenerator::generateTerrain(
		-(terrainData.chunkWidth * terrainData.chunkRows / 2 * terrainData.gridSize), 
		-1, 
		-(terrainData.chunkLength * terrainData.chunkCols / 2 * terrainData.gridSize),
		terrainData,
		models, grassTexture, 8.0f,
		terrainGenerator.get(), 1
	);

	loadModelsFromDirectory("models", models);

	createSkyModel(sky);

	for (Mesh& mesh : sky.meshes) {
		createVertexBuffer(mesh);
		createIndexBuffer(mesh);
	}

	for (size_t i = 0; i < models.size(); i++) {
		for (Mesh& mesh : models[i].meshes) {
			createVertexBuffer(mesh);
			createIndexBuffer(mesh);
			computeAABB(mesh);
		}
	}

	createDescriptorPool();

	createShaderBuffers(sky, MAX_FRAMES_IN_FLIGHT);
	createShaderBuffers(models, MAX_FRAMES_IN_FLIGHT);
	createDescriptorSets(sky, MAX_FRAMES_IN_FLIGHT);
	createDescriptorSets(models, MAX_FRAMES_IN_FLIGHT);
}

void AetherEngine::changeState(GameState newGameState) {
	if (gameContext.currentGameState == newGameState) {
		std::cout << 
			"new state is same as current state: " << 
			static_cast<uint32_t>(newGameState) << "\n";
		return;
	}

	gameContext.clearInputs();
	gameContext.currentGameState = newGameState;

	switch (newGameState) {
	case GameState::MAIN_MENU:
		stackedWidget->setCurrentWidget(mainMenuWidget);
		break;
	case GameState::IN_GAME:
		stackedWidget->setCurrentWidget(inGameWidget);
		break;
	case GameState::EXIT:
		QCoreApplication::quit();
		break;
	default:
		break;
	}
}
void AetherEngine::mainLoop()
{
	std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point currentTime;
	double deltaTime;
	double timeSinceLaunch = 0.0f;
	
	uint32_t counter = 0;
	double accumulator = 0;
	double fps = 0;
	bool fpsMenu = 0;

	while (gameContext.currentGameState != GameState::EXIT) {
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - previousTime).count();
		previousTime = currentTime;
		timeSinceLaunch += deltaTime;

		/*if (deltaTime > 0.0002) {
			std::cout << "deltaTime: " << deltaTime << "\n";
		}*/

		QCoreApplication::processEvents();

		if (gameContext.requestedGameState != GameState::NONE) {
			changeState(gameContext.requestedGameState);
			gameContext.requestedGameState = GameState::NONE;
		}

		if (gameContext.currentGameState == GameState::IN_GAME) {
			character.handleInGamePlayerInput(gameContext);
			if (!character.isInteracting) {
				character.handleCharacterMovement(gameContext, deltaTime, gravity, models);
			}

			character.camera.addRotationDelta(inGameWindow->latestMouseDx, inGameWindow->latestMouseDy);
			inGameWindow->latestMouseDx = 0.0;
			inGameWindow->latestMouseDy = 0.0;

			character.camera.interpolateRotation(1.0);
			if (inGameWindow->isActive()) {
				QCursor::setPos(inGameWindow->centerPos);
			}

			// FPS meter
			if (fpsMenu) {
				counter++;
				accumulator += deltaTime;
				if (accumulator >= 1.0) {
					fps = 1 / (accumulator / counter);
					counter = 0;
					accumulator = 0;
					std::cout << "fps: " << fps << "\n";
				}
			}

			drawFrame(timeSinceLaunch, deltaTime);
		}
	}

	vkDeviceWaitIdle(vkInit.device);
}

void AetherEngine::cleanupModels(std::vector<Model>& models) const
{
	for (Model& model : models) {
		cleanupModel(model);
	}
}
void AetherEngine::cleanupModel(Model& model) const
{
	for (Mesh& mesh : model.meshes) {
		cleanupMesh(mesh);
	}
}
void AetherEngine::cleanupMesh(Mesh& mesh) const
{
	vkDestroyBuffer(vkInit.device, mesh.indexBuffer, nullptr);
	vkFreeMemory(vkInit.device, mesh.indexBufferMemory, nullptr);

	vkDestroyBuffer(vkInit.device, mesh.vertexBuffer, nullptr);
	vkFreeMemory(vkInit.device, mesh.vertexBufferMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(vkInit.device, mesh.UBOBuffers[i], nullptr);
		vkFreeMemory(vkInit.device, mesh.UBOBuffersMemory[i], nullptr);

		if (mesh.bones.size() > 0) {
			vkDestroyBuffer(vkInit.device, mesh.boneSSBOBuffers[i], nullptr);
			vkFreeMemory(vkInit.device, mesh.boneSSBOBuffersMemory[i], nullptr);
		}
	}

	// cleanupMaterial(mesh.material);
}
void AetherEngine::cleanupMaterial(Material& material) const
{
	cleanupTexture(material.diffuseTexture);
	cleanupTexture(material.normalTexture);
	cleanupTexture(material.specularTexture);
	cleanupTexture(material.emissiveTexture);
}
void AetherEngine::cleanupTexture(Texture& texture) const
{
	if (texture.imageView) {
		vkDestroyImageView(vkInit.device, texture.imageView, nullptr);
		texture.imageView = VK_NULL_HANDLE;
	}
	if (texture.image) {
		vkDestroyImage(vkInit.device, texture.image, nullptr);
		texture.image = VK_NULL_HANDLE;
	}
	if (texture.imageMemory) {
		vkFreeMemory(vkInit.device, texture.imageMemory, nullptr);
		texture.imageMemory = VK_NULL_HANDLE;
	}
	if (texture.sampler) {
		vkDestroySampler(vkInit.device, texture.sampler, nullptr);
		texture.sampler = VK_NULL_HANDLE;
	}
}

void AetherEngine::cleanupMemory()
{
	cleanupModels(models);
	cleanupModel(sky);
	cleanupTexture(grassTexture);
	cleanupTexture(dummyTexture);

	cleanupSwapchain();

	// will free all allocated descriptor sets from this pool
	vkDestroyDescriptorPool(vkInit.device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(vkInit.device, descriptorSetLayout, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(vkInit.device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(vkInit.device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(vkInit.device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(vkInit.device, commandPool, nullptr);

	vkDestroyDevice(vkInit.device, nullptr);

	if (vkInit.enableValidationLayers) {
		vkInit.DestroyDebugUtilsMessengerEXT(vkInit.instance, vkInit.debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(vkInit.instance, vkInit.surface, nullptr);
}

void AetherEngine::restrictCharacterMovement(Camera& camera)
{
	glm::vec3 cameraPosition = camera.getLookFrom();

	glm::vec3 retrictPoint0 = glm::vec3(35.0, 30.0, 35.0);
	glm::vec3 retrictPoint1 = glm::vec3(-35.0, 3.0, -35.0);

	if (cameraPosition.x > retrictPoint0.x) {
		camera.setLookFrom(glm::vec3(retrictPoint0.x, cameraPosition.y, cameraPosition.z));
		cameraPosition = camera.getLookFrom();
	}
	if (cameraPosition.x < retrictPoint1.x) {
		camera.setLookFrom(glm::vec3(retrictPoint1.x, cameraPosition.y, cameraPosition.z));
		cameraPosition = camera.getLookFrom();
	}
	if (cameraPosition.y > retrictPoint0.y) {
		camera.setLookFrom(glm::vec3(cameraPosition.x, retrictPoint0.y, cameraPosition.z));
		cameraPosition = camera.getLookFrom();
	}
	if (cameraPosition.y < retrictPoint1.y) {
		camera.setLookFrom(glm::vec3(cameraPosition.x, retrictPoint1.y, cameraPosition.z));
		cameraPosition = camera.getLookFrom();
	}
	if (cameraPosition.z > retrictPoint0.z) {
		camera.setLookFrom(glm::vec3(cameraPosition.x, cameraPosition.y, retrictPoint0.z));
		cameraPosition = camera.getLookFrom();
	}
	if (cameraPosition.z < retrictPoint1.z) {
		camera.setLookFrom(glm::vec3(cameraPosition.x, cameraPosition.y, retrictPoint1.z));
		cameraPosition = camera.getLookFrom();
	}
}

std::string AetherEngine::createPuzzleEquation(std::string name, int32_t& answer)
{
	std::stringstream puzzleEquation;

	if (name == "easy") {
		// Generate an easy equation: a + b or a - b
		int a = rand() % 50 + 1; // Random number between 1 and 10
		int b = rand() % 50 + 1;
		char op = rand() % 2 == 0 ? '+' : '-';

		if (op == '+') {
			answer = a + b;
		}
		else {
			answer = a - b;
		}

		puzzleEquation << a << " " << op << " " << b;
	}
	else if (name == "medium") {
		// Generate a medium equation: a * b or a / b (where b divides a evenly)
		int a = rand() % 14 + 1;
		int b = rand() % 10 + 1; // Avoid zero to prevent division by zero
		char op = rand() % 2 == 0 ? '*' : '/';
		if (op == '/') {
			// Ensure that the division is an integer division
			a = a * b; // Adjust 'a' to make it divisible by 'b'
		}

		if (op == '*') {
			answer = a * b;
		}
		else {
			a = a * b;
			answer = a / b;
		}

		puzzleEquation << a << " " << op << " " << b;
	}
	else if (name == "hard") {
		// Generate a hard equation: (a + b) * c or (a - b) / c (where c divides the result evenly)
		int a = rand() % 12 + 2;
		int b = rand() % 12 + 2;
		int c = rand() % 12 + 2; // Avoid zero to prevent division by zero
		char op1 = rand() % 2 == 0 ? '+' : '-';
		char op2 = rand() % 2 == 0 ? '*' : '/';
		if (op2 == '/') {
			// Ensure that the division is an integer division
			int a_b_sum = (rand() % 12 + 4) * c;
			if (op1 == '+') {
				a = (a_b_sum - std::min(rand() % 12 + 4, a_b_sum));
				b = a_b_sum - a;
			}
			else if (op1 == '-') {
				a = (a_b_sum - std::min(rand() % 12 + 4, a_b_sum));
				b = a_b_sum + a;
			}
		}

		if      (op1 == '+' && op2 == '*') {
			answer = (a + b) * c;
		}
		else if (op1 == '+' && op2 == '/'){
			answer = (a + b) / c;
		}
		else if (op1 == '-' && op2 == '*') {
			answer = (a - b) * c;
		}
		else if (op1 == '-' && op2 == '/') {
			answer = (a - b) / c;
		}

		puzzleEquation << "(" << a << " " << op1 << " " << b << ") " << op2 << " " << c;
	}
	
	return puzzleEquation.str();
}

// reading byte code files and returning its bytes
std::vector<char> AetherEngine::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file: " + filename);
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

// finding most appropriate memory type depending on buffer and application properties
uint32_t AetherEngine::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(vkInit.physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

// command pool for specific queue family
void AetherEngine::createCommandPool()
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = vkInit.queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(vkInit.device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

// Creating fences and semaphores
void AetherEngine::createSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	// Fence will be in signaled state after creation
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(vkInit.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(vkInit.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(vkInit.device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame: " + i);
		}
	}
}

// wrapping shader
VkShaderModule AetherEngine::createShaderModule(const std::vector<char>& code) const
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(vkInit.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}