#include "pch.h"
#include "AetherEngine.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

void AetherEngine::run()
{
	prepareUI();
	vkInit.initializeVulkan(&qVulkanInstance);
	initVMA();
	prepareResources();
	mainLoop();
	cleanupMemory();
}

void AetherEngine::prepareUI() 
{
	setWindowSize();
	createMainWindow();
	createInGameWindow();

	createMainMenuWidget();
	createSettingsMenuWidget();
	createPauseMenuRenderer();

	mainWindow->addWidget(mainMenuWidget);
	mainWindow->addWidget(inGameWidget);
	//mainWindow->addWidget(inGameContainerWidget);
	mainWindow->addWidget(settingsMenuWidget);
	mainWindow->show();

	gameContext.requestedGameState = GameState::MAIN_MENU;
	gameContext.requestedGameState = GameState::IN_GAME_TESTING;
	gameContext.requestedGameState = GameState::DUNGEON_EXPLORATION;
	gameContext.clearInputs();
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
	createTextureFromPath("textures/stone_wall_floor_3.png", stone_wall_floor_2_texture);
	createTextureFromPath("textures/stone_floor_floor_2.png", stone_floor_floor_2_texture);
	createTextureFromPath("textures/floor_background_floor_2.png", floor_background_floor_2_texture);
	createSolidColorTexture({ 0, 0, 0, 0 }, 1, 1, transparentTexture);
	createSolidColorTexture({ 0, 0, 0, 0 }, windowWidth, windowHeight, pauseMenuTexture);

	//loadModelsFromDirectory("models", models);
	
	//TerrainData terrainData = {
	//	100, 100, // chunkWidth, chunkLength
	//	4, 4,     // chunkRows, chunkCols
	//	2.0f,     // gridSize
	//	0.1f,     // scale
	//	0.2f,     // height
	//};
	//terrainGenerator = std::make_unique<TerrainGenerator>(1);
	//TerrainGenerator::generateTerrain(
	//	-(terrainData.chunkWidth * terrainData.chunkRows / 2 * terrainData.gridSize),
	//	0,
	//	-(terrainData.chunkLength * terrainData.chunkCols / 2 * terrainData.gridSize),
	//	terrainData,
	//	models, grassTexture, 8.0f,
	//	terrainGenerator.get(), 1
	//);

	Dungeon::createDungeonFloor(
		gameContext.dungeonFloor, models, 
		stone_floor_floor_2_texture, stone_wall_floor_2_texture
	);
	Dungeon::enterDungeonFloor(gameContext.dungeonFloor, gameContext, character);

	pauseMenuModel.push_back(ModelManager::createQuad(
		{ -1.0f, -1.0f, 0.0f }, { 2.0f, 2.0f }, 
		{ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, 
		glm::vec3(0.5f), 
		pauseMenuTexture
	));

	ModelManager::createSkyModel(sky);

	createDescriptorPool(models, pauseMenuModel);

	computeAABB_createVertexIndexBuffers(sky);
	computeAABB_createVertexIndexBuffers(models);
	computeAABB_createVertexIndexBuffers(pauseMenuModel);

	createShaderBuffers(sky, MAX_FRAMES_IN_FLIGHT);
	createShaderBuffers(models, MAX_FRAMES_IN_FLIGHT);
	//createShaderBuffers(uiModels, MAX_FRAMES_IN_FLIGHT);

	createDescriptorSets(sky, MAX_FRAMES_IN_FLIGHT);
	createDescriptorSets(models, MAX_FRAMES_IN_FLIGHT);
	createDescriptorSets(pauseMenuModel, MAX_FRAMES_IN_FLIGHT);
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
void AetherEngine::onMainWindowResized(int width, int height) {
	isFramebufferResized = true;
	windowWidth = width;
	windowHeight = height;

	//std::cout << "onFramebufferResized: " << width << " " << height << "\n";
	//std::cout << "center pos: " << gameContext.windowCenterPos.x() << " " << gameContext.windowCenterPos.y() << "\n";
	character.camera.setViewportSize(windowWidth, windowHeight);
	pauseMenuRenderer->resize(QSize(windowWidth, windowHeight));
}
void AetherEngine::onMainWindowMoved(int x, int y) {
	//pauseMenuView->getQuickWindow()->setPosition(x, y);
	//pauseMenuView->setPosition(inGameWindow->frameGeometry().topLeft() * -1);
}
void AetherEngine::onInGameWindowMoved(int x, int y) {
	//pauseMenuView->setPosition(x, y);
}
void AetherEngine::onInGameWindowLostFocus() {
	//std::cout << "lost focus\n";
	//pauseMenuView->show();
}

void AetherEngine::setWindowSize()
{
	QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
	windowWidth = screenGeometry.width() / 1.5;
	windowHeight = screenGeometry.height() / 1.5;
}
void AetherEngine::createMainMenuWidget() {
	mainMenuWidget = new MainMenuWidget();
	mainMenuWidget->resize(windowWidth, windowHeight);

	connect(mainMenuWidget, &MainMenuWidget::startGame, [this]() {
		gameContext.requestedGameState = GameState::DUNGEON_EXPLORATION;
		//gameContext.requestedGameState = GameState::IN_GAME_TESTING;
		});

	connect(mainMenuWidget, &MainMenuWidget::openSettings, [this]() {
		gameContext.requestedGameState = GameState::SETTINGS_MENU;
		});

	connect(mainMenuWidget, &MainMenuWidget::exitGame, [this]() {
		gameContext.requestedGameState = GameState::EXIT;
		});
}
void AetherEngine::createSettingsMenuWidget() {
	settingsMenuWidget = new SettingsMenuWidget();
	settingsMenuWidget->resize(windowWidth, windowHeight);

	connect(settingsMenuWidget, &SettingsMenuWidget::returnToMainMenu, [this]() {
		gameContext.requestedGameState = GameState::MAIN_MENU;
		});
}
void AetherEngine::createPauseMenuRenderer() {
	pauseMenuRenderer = new UserInterfaceRenderer();

	pauseMenuRenderer->initialize(QSize(windowWidth, windowHeight), "qml/PauseMenu.ui.qml");
	pauseMenuRenderer->setParent(mainWindow);
	inGameWindow->setPauseMenuRenderer(pauseMenuRenderer);
	PauseMenuSlotHandler* pauseMenuSlotHandler = new PauseMenuSlotHandler(gameContext, this);

	auto rootItem = pauseMenuRenderer->getRootItem();

	connect(rootItem, SIGNAL(resumeGameClicked()), pauseMenuSlotHandler, SLOT(onResumeGameClicked()));
	connect(rootItem, SIGNAL(openSettingsClicked()), pauseMenuSlotHandler, SLOT(onOpenSettingsClicked()));
	connect(rootItem, SIGNAL(openMainMenuClicked()), pauseMenuSlotHandler, SLOT(onOpenMainMenuClicked()));
	connect(rootItem, SIGNAL(exitGameClicked()), pauseMenuSlotHandler, SLOT(onExitGameClicked()));
}
void AetherEngine::updateInGameOverlay() {
	if (!inGameOverlayRenderer) return;

	auto rootItem = inGameOverlayRenderer->getRootItem();
	if (!rootItem) return;

	// Update player health
	QObject* playerHealth = rootItem->findChild<QObject*>("playerHealth");
	if (playerHealth)
		playerHealth->setProperty("text", tr("Health: %1").arg(character.health));

	// Update mob health
	QObject* mobHealth = rootItem->findChild<QObject*>("mobHealth");
	if (mobHealth) {
		if (!gameContext.currentRoom->mobs.empty()) {
			mobHealth->setProperty("text", tr("Mob health: %1").arg(gameContext.currentRoom->mobs[0].health));
			mobHealth->setProperty("visible", true);
		}
		else {
			mobHealth->setProperty("visible", false);
		}
	}
}

void AetherEngine::createMainWindow()
{
	mainWindow = new MainWindow(gameContext);
	mainWindow->resize(windowWidth, windowHeight);
	mainWindow->setWindowTitle("Aether");
	mainWindow->setWindowIcon(QIcon("textures/granite_square.png"));

	stackedWidget = mainWindow->getStackedWidget();

	connect(mainWindow, &MainWindow::resized, this,
		&AetherEngine::onMainWindowResized
	);

	connect(mainWindow, &MainWindow::moved, this,
		&AetherEngine::onMainWindowMoved
	);

	connect(mainWindow, &MainWindow::closed, [this]() {
		gameContext.requestedGameState = GameState::EXIT;
		});
}
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

	inGameWindow = new InGameWindow(&qVulkanInstance, character, gameContext);
	inGameWindow->setParent(mainWindow->windowHandle());
	inGameWindow->setFlags(Qt::FramelessWindowHint);
	inGameWindow->resize(windowWidth, windowHeight);
	inGameWindow->setTitle("Aether in game");
	inGameWindow->create();

	vkInit.surface = qVulkanInstance.surfaceForWindow(inGameWindow);
	if (vkInit.surface == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to create Vulkan surface for Qt window.");
	}

	inGameWidget = QWidget::createWindowContainer(inGameWindow);
	inGameWidget->resize(inGameWindow->size());

	connect(inGameWindow, &InGameWindow::moved, this,
		&AetherEngine::onInGameWindowMoved
	);

	connect(inGameWindow, &InGameWindow::lostFocus, this,
		&AetherEngine::onInGameWindowLostFocus
	);

	connect(inGameWindow, &InGameWindow::closed, [this]() {
		gameContext.requestedGameState = GameState::EXIT;
		});
}

void AetherEngine::changeState(GameState newGameState) {
	if (gameContext.currentGameState == newGameState) {
		std::cout <<
			"new state is same as current state: " <<
			static_cast<uint32_t>(newGameState) << "\n";
		return;
	}

	// leaving current state
	switch (gameContext.currentGameState) {
	case GameState::MAIN_MENU:
		break;
	case GameState::SETTINGS_MENU:
		break;
	case GameState::IN_GAME_TESTING:
		QApplication::setOverrideCursor(Qt::ArrowCursor);
		break;
	case GameState::PAUSED:
		break;
	case GameState::EXIT:
		break;
	default:
		break;
	}

	/*std::cout << 
		"currentGameState: " << static_cast<uint32_t>(gameContext.currentGameState) << 
		" newGameState: "    << static_cast<uint32_t>(newGameState) << "\n";*/
	gameContext.clearInputs();
	gameContext.currentGameState = newGameState;

	// entering new state
	switch (newGameState) {
	case GameState::MAIN_MENU:
		stackedWidget->setCurrentWidget(mainMenuWidget);
		break;
	case GameState::SETTINGS_MENU:
		stackedWidget->setCurrentWidget(settingsMenuWidget);
		break;
	case GameState::IN_GAME_TESTING:
		QApplication::setOverrideCursor(Qt::BlankCursor);
		character.camera._isFirstMouse = true;
		stackedWidget->setCurrentWidget(inGameWidget);
		break;
	case GameState::DUNGEON_EXPLORATION:
		stackedWidget->setCurrentWidget(inGameWidget);
		break;
	case GameState::COMBAT_PLAYER_TURN:
		stackedWidget->setCurrentWidget(inGameWidget);
		break;
	case GameState::PAUSED:
		break;
	case GameState::EXIT:
		QCoreApplication::quit();
		break;
	default:
		break;
	}
}
void AetherEngine::handleInDungeonState(double deltaTime, double timeSinceLaunch)
{
	character.handleInDungeonPlayerInput(gameContext);
}
void AetherEngine::handleInGameTestingState(double deltaTime, double timeSinceLaunch, bool fpsMenu)
{
	character.handleInGamePlayerInput(gameContext);
	if (!character.isInteracting) {
		character.handleCharacterMovement(gameContext, deltaTime, gravity, models);
	}

	// skipping qt mouse movements on the first frame
	character.camera.handleRotation(inGameWindow->latestMouseDx, inGameWindow->latestMouseDy);

	if (inGameWindow->isActive()) {
		QCursor::setPos(gameContext.windowCenterPos);
		//std::cout << "set mouse pos 0: " << gameContext.windowCenterPos.x() << " " << gameContext.windowCenterPos.y() << "\n";
	}

	// FPS meter
	static uint32_t counter = 0;
	static double accumulator = 0.0;
	static double fps = 0.0;
	
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
}

void AetherEngine::mainLoop()
{
	std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point currentTime;
	double deltaTime;
	double timeSinceLaunch = 0.0f;

	bool fpsMenu = 0;

	while (gameContext.currentGameState != GameState::EXIT) {
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - previousTime).count();
		previousTime = currentTime;
		timeSinceLaunch += deltaTime;

		QCoreApplication::processEvents();

		if (!mainWindow or !inGameWindow) {
			break;
		}

		if (gameContext.requestedGameState != GameState::NONE) {
			changeState(gameContext.requestedGameState);
			gameContext.requestedGameState = GameState::NONE;
		}

		if (gameContext.currentGameState == GameState::SETTINGS_MENU) {
			if (gameContext.keyboardKeys[Qt::Key_Escape]) {
				gameContext.requestedGameState = GameState::MAIN_MENU;
			}
		}

		if (gameContext.currentGameState == GameState::PAUSED) {
			if (gameContext.keyboardKeys[Qt::Key_Escape]) {
				gameContext.requestedGameState = GameState::DUNGEON_EXPLORATION;
			}
		}

		// drawing frames after window resize, for removing black borders after resize
		if (gameContext.currentGameState != GameState::MAIN_MENU && isFramebufferResized) {
			for (size_t i = 0; i < 1; i++) {
				drawFrame(timeSinceLaunch, deltaTime);
			}
		}

		if (gameContext.currentGameState == GameState::DUNGEON_EXPLORATION and inGameWindow->isExposed()) {
			handleInDungeonState(deltaTime, timeSinceLaunch);
		}
		
		if (gameContext.currentGameState == GameState::IN_GAME_TESTING and inGameWindow->isExposed()) {
			handleInGameTestingState(deltaTime, timeSinceLaunch, fpsMenu);
		}

		if (gameContext.currentGameState == GameState::DUNGEON_EXPLORATION      ||
			gameContext.currentGameState == GameState::IN_GAME_TESTING ||
			gameContext.currentGameState == GameState::COMBAT_PLAYER_TURN       ||
			gameContext.currentGameState == GameState::PAUSED) {

			drawFrame(timeSinceLaunch, deltaTime);
		}
	}

	vkDeviceWaitIdle(vkInit.device);
}

void AetherEngine::cleanupMemory()
{
	cleanupModel(sky);
	cleanupModels(models);
	cleanupModels(pauseMenuModel);

	cleanupTexture(stone_wall_floor_1_texture);
	cleanupTexture(stone_wall_floor_2_texture);
	cleanupTexture(stone_wall_floor_3_texture);
	cleanupTexture(stone_floor_floor_1_texture);
	cleanupTexture(stone_floor_floor_2_texture);
	cleanupTexture(stone_floor_floor_3_texture);
	cleanupTexture(floor_background_floor_2_texture);

	cleanupTexture(grassTexture);
	cleanupTexture(transparentTexture);
	cleanupTexture(pauseMenuTexture);
	cleanupTexture(depthTexture);
	cleanupTexture(msaaTexture);

	cleanupSwapchain();

	cleanedImages.clear();
	cleanedImageViews.clear();
	cleanedImageSamplers.clear();

	if (vkInit.enableValidationLayers) {
		std::cout << "createdBuffers: " << createdBuffers << "\n";
		std::cout << "destroyedVmaAllocations: " << destroyedVmaAllocations << "\n";
	}

	// will free all allocated descriptor sets from this pool
	vkDestroyDescriptorPool(vkInit.device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(vkInit.device, descriptorSetLayout, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(vkInit.device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(vkInit.device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(vkInit.device, inFlightFences[i], nullptr);
	}

	vmaDestroyAllocator(vmaAllocator);
	vkDestroyCommandPool(vkInit.device, commandPool, nullptr);

	vkDestroyDevice(vkInit.device, nullptr);

	if (vkInit.enableValidationLayers) {
		vkInit.DestroyDebugUtilsMessengerEXT(vkInit.instance, vkInit.debugMessenger, nullptr);
	}

	// surface is handled by QVulkanInstance itself and shouldn't be destroyed manually
	/*if (vkInit.surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(vkInit.instance, vkInit.surface, nullptr);
		vkInit.surface = VK_NULL_HANDLE;
	}*/
}

void AetherEngine::cleanupModels(std::vector<Model>& models) const
{
	for (Model& model : models) {
		cleanupModel(model);
	}
	models.clear();
}
void AetherEngine::cleanupModel(Model& model) const
{
	for (Mesh& mesh : model.meshes) {
		cleanupMesh(mesh);
	}
	model.meshes.clear();
}
void AetherEngine::cleanupMesh(Mesh& mesh) const
{
	mesh.vertices.clear();
	mesh.indices.clear();
	mesh.transform = glm::mat4(1.0f);
	mesh.aabb = {};

	mesh.bones.clear();
	mesh.boneMap.clear();

	if (mesh.descriptorSets != std::vector<VkDescriptorSet>(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE)) {
		vkFreeDescriptorSets(
			vkInit.device, descriptorPool, 
			static_cast<uint32_t>(mesh.descriptorSets.size()), 
			mesh.descriptorSets.data()
		);
		mesh.descriptorSets = std::vector<VkDescriptorSet>(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
	}
	if (mesh.vertexBufferAllocation != VK_NULL_HANDLE) {
		vmaDestroyBuffer(vmaAllocator, mesh.vertexBuffer, mesh.vertexBufferAllocation);
		mesh.vertexBuffer = VK_NULL_HANDLE;
		mesh.vertexBufferAllocation = VK_NULL_HANDLE;
		destroyedVmaAllocations += 1;
	}

	if (mesh.indexBufferAllocation != VK_NULL_HANDLE) {
		vmaDestroyBuffer(vmaAllocator, mesh.indexBuffer, mesh.indexBufferAllocation);
		mesh.indexBuffer = VK_NULL_HANDLE;
		mesh.indexBufferAllocation = VK_NULL_HANDLE;
		destroyedVmaAllocations += 1;
	}

	cleanupShaderBuffers(mesh);

	cleanupMaterial(mesh.material);
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
	if (texture.imageView && cleanedImageViews.insert(texture.imageView).second) {
		vkDestroyImageView(vkInit.device, texture.imageView, nullptr);
		texture.imageView = VK_NULL_HANDLE;
	}
	if (texture.sampler && cleanedImageSamplers.insert(texture.sampler).second) {
		vkDestroySampler(vkInit.device, texture.sampler, nullptr);
		texture.sampler = VK_NULL_HANDLE;
	}
	if (texture.image && texture.vmaAllocation && cleanedImages.insert(texture.image).second) {
		vmaDestroyImage(vmaAllocator, texture.image, texture.vmaAllocation);
		texture.image = VK_NULL_HANDLE;
		texture.vmaAllocation = VK_NULL_HANDLE;
		destroyedVmaAllocations += 1;
	}

	texture.width = 0;
	texture.height = 0;
	texture.mipLevels = 0;
}
void AetherEngine::cleanupShaderBuffers(std::vector<Model>& models) const
{
	for (Model& model : models) {
		cleanupShaderBuffers(model);
	}
}
void AetherEngine::cleanupShaderBuffers(Model& model) const
{
	for (Mesh& mesh : model.meshes) {
		cleanupShaderBuffers(mesh);
	}
}
void AetherEngine::cleanupShaderBuffers(Mesh& mesh) const
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (mesh.UBOAllocations[i] != VK_NULL_HANDLE) {
			vmaDestroyBuffer(vmaAllocator, mesh.UBOBuffers[i], mesh.UBOAllocations[i]);
			mesh.UBOBuffers[i] = VK_NULL_HANDLE;
			mesh.UBOAllocations[i] = VK_NULL_HANDLE;
			destroyedVmaAllocations += 1;
		}

		if (mesh.boneSSBOAllocations[i] != VK_NULL_HANDLE) {
			vmaDestroyBuffer(vmaAllocator, mesh.boneSSBOBuffers[i], mesh.boneSSBOAllocations[i]);
			mesh.boneSSBOBuffers[i] = VK_NULL_HANDLE;
			mesh.boneSSBOAllocations[i] = VK_NULL_HANDLE;
			destroyedVmaAllocations += 1;
		}
	}
}

void AetherEngine::restrictCharacterMovement(Camera& camera)
{
	glm::vec3 cameraPosition = camera.getPosition();

	glm::vec3 retrictPoint0 = glm::vec3(35.0, 30.0, 35.0);
	glm::vec3 retrictPoint1 = glm::vec3(-35.0, 3.0, -35.0);

	if (cameraPosition.x > retrictPoint0.x) {
		camera.setPosition(glm::vec3(retrictPoint0.x, cameraPosition.y, cameraPosition.z));
		cameraPosition = camera.getPosition();
	}
	if (cameraPosition.x < retrictPoint1.x) {
		camera.setPosition(glm::vec3(retrictPoint1.x, cameraPosition.y, cameraPosition.z));
		cameraPosition = camera.getPosition();
	}
	if (cameraPosition.y > retrictPoint0.y) {
		camera.setPosition(glm::vec3(cameraPosition.x, retrictPoint0.y, cameraPosition.z));
		cameraPosition = camera.getPosition();
	}
	if (cameraPosition.y < retrictPoint1.y) {
		camera.setPosition(glm::vec3(cameraPosition.x, retrictPoint1.y, cameraPosition.z));
		cameraPosition = camera.getPosition();
	}
	if (cameraPosition.z > retrictPoint0.z) {
		camera.setPosition(glm::vec3(cameraPosition.x, cameraPosition.y, retrictPoint0.z));
		cameraPosition = camera.getPosition();
	}
	if (cameraPosition.z < retrictPoint1.z) {
		camera.setPosition(glm::vec3(cameraPosition.x, cameraPosition.y, retrictPoint1.z));
		cameraPosition = camera.getPosition();
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