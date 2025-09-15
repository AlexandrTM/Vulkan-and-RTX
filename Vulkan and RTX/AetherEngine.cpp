#include "pch.h"
#include "AetherEngine.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

void AetherEngine::run()
{
	/*std::chrono::high_resolution_clock::time_point currentTime;
	float deltaTime;
	std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();

	

	currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - previousTime).count();
	if (deltaTime > 1e-9) {
		std::cout << "time to create: " << deltaTime << "\n";
	}*/

	//Equations::debugEquations(1000, 1.0);

	prepareUI();
	vkInit.initializeVulkan(&qVulkanInstance);
	prepareResources();
	mainLoop();
	cleanupMemory();
}

void AetherEngine::prepareUI() 
{
	setWindowSize();
	createMainWindow();
	createInGameWindow();

	mainWindow->addWidget(inGameWidget);
	mainWindow->show();

	inGameWindow->setKeyboardGrabEnabled(true);

	gameContext.requestedGameState = GameState::COMBAT_PLAYER_SOLVE_EQUATION;
	gameContext.requestedGameState = GameState::COMBAT_PLAYER_SELECT_EQUATION;
	gameContext.requestedGameState = GameState::MAIN_MENU;
	gameContext.requestedGameState = GameState::DUNGEON_EXPLORATION;
	gameContext.requestedGameState = GameState::IN_GAME_TESTING;
	gameContext.requestedGameState = GameState::PLAYER_DEAD;
	gameContext.clearInputs();
}

void AetherEngine::prepareResources()
{
	createDescriptorSetLayout(descriptorSetLayout);

	createPipelinesAndSwapchain();

	modelManager.createColorTexture(swapchainImageFormat, swapchainExtent.width, swapchainExtent.height, msaaTexture);
	modelManager.createDepthTexture(swapchainExtent.width, swapchainExtent.height, depthTexture);
	createSwapchainFramebuffers();
	bufferManager.createCommandBuffers(commandBuffers);
	createSyncObjects();

	grassTexture = modelManager.loadTextureFromPath("textures/grass001.png");
	floor_background = modelManager.loadTextureFromPath("textures/floor_background_floor_2.png");
	modelManager.createSolidColorTexture({ 0, 0, 0, 0 }, 1, 1, transparentTexture);
	notFoundTexture = modelManager.loadTextureFromPath("textures/notFoundTexture.png");

	//loadModelsFromFolder("models", models);
	
	TerrainData terrainData = {
		30, 30,   // chunkWidth, chunkLength
		4, 4,     // chunkRows, chunkCols
		8.0f,     // gridSize
		0.1f,     // scale
		0.0f,     // height
	};
	terrainGenerator = std::make_unique<TerrainGenerator>();
	terrainGenerator->generateTerrain(
		-(terrainData.chunkWidth * terrainData.chunkRows / 2 * terrainData.gridSize),
		-0.01,
		-(terrainData.chunkLength * terrainData.chunkCols / 2 * terrainData.gridSize),
		terrainData,
		models, floor_background, 8.0f,
		1 // not used
	);
	
	ModelPrimitives::createSkyModel(sky);

	/*size_t meshesNum = 0;
	for (const Model& model : models) {
		meshesNum += model.meshes.size();
	}*/
	createDescriptorPool(33 * 15 * 2, 33 * 15 * 2, descriptorPool);

	loadUIElements();
	
	bufferManager.computeAABB_createVertexIndexBuffers(sky);
	bufferManager.createShaderBuffers(sky, MAX_FRAMES_IN_FLIGHT);
	createDescriptorSets(sky, MAX_FRAMES_IN_FLIGHT);

	bufferManager.computeAABB_createVertexIndexBuffers(models);
	bufferManager.createShaderBuffers(models, MAX_FRAMES_IN_FLIGHT);
	createDescriptorSets(models, MAX_FRAMES_IN_FLIGHT);
}

void AetherEngine::onMainWindowResized(int width, int height) {
	isFramebufferResized = true;
	windowWidth = width;
	windowHeight = height;

	//std::cout << "onFramebufferResized: " << width << " " << height << "\n";
	//std::cout << "center pos: " << gameContext.windowCenterPos.x() << " " << gameContext.windowCenterPos.y() << "\n";
	character.camera.setViewportSize(windowWidth, windowHeight);

	for (auto& [id, elem] : uiMap) {
		if (elem.renderer) {
			elem.renderer->resize(QSize(windowWidth, windowHeight));
		}
	}
}
void AetherEngine::onMainWindowMoved(int x, int y) {
	//pauseMenuView->getQuickWindow()->setPosition(x, y);
	//pauseMenuView->setPosition(inGameWindow->frameGeometry().topLeft() * -1);
}
void AetherEngine::onInGameWindowMoved(int x, int y) {
	//pauseMenuView->setPosition(x, y);
	if (uiMap[uiElementId::SolveEquation].renderer) {
		uiMap[uiElementId::SolveEquation].renderer->getQuickWindow()->setPosition(inGameWindow->position() + QPoint(x, y));
	}
}
void AetherEngine::onInGameWindowLostFocus() {
	//std::cout << "lost focus\n";
	//pauseMenuView->show();
}

void AetherEngine::setWindowSize()
{
	//QRect screenGeometry = QApplication::primaryScreen()->geometry();
	QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
	//std::cout << "screen name: " << QApplication::screens()[0]->name().toStdString() << "\n";
	windowWidth = screenGeometry.width() / 1.5;
	windowHeight = screenGeometry.height() / 1.5;
}

UserInterfaceElement AetherEngine::createUIElement(
	const QString& qmlPath, size_t windowWidth, size_t windowHeight, QObject* parent
) {
	UserInterfaceElement uiElement;

	modelManager.createSolidColorTexture({ 0, 0, 0, 0 }, windowWidth, windowHeight, uiElement.texture);
	uiElement.model = ModelPrimitives::createQuad(
		{ -1.0f, -1.0f, 0.0f }, { 2.0f, 2.0f },
		{ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f },
		glm::vec3(0.5f),
		uiElement.texture
	);
	bufferManager.computeAABB_createVertexIndexBuffers(uiElement.model);
	createDescriptorSets(uiElement.model, MAX_FRAMES_IN_FLIGHT);

	uiElement.renderer = std::make_unique<UserInterfaceRenderer>();
	uiElement.renderer->loadQml(QSize(windowWidth, windowHeight), qmlPath);
	uiElement.renderer->setParent(parent);

	return uiElement;
}
void AetherEngine::changeUIElementSize(
	UserInterfaceElement& uiElement, size_t windowWidth, size_t windowHeight
) {
	/*cleanupTexture(pauseMenuTexture);*/

	cleanupModel(uiElement.model);
	
	modelManager.createSolidColorTexture({ 0, 0, 0, 0 }, windowWidth, windowHeight, uiElement.texture);
	uiElement.model = ModelPrimitives::createQuad(
		{ -1.0f, -1.0f, 0.0f }, { 2.0f, 2.0f },
		{ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f },
		glm::vec3(0.5f),
		uiElement.texture
	);
	bufferManager.computeAABB_createVertexIndexBuffers(uiElement.model);
	createDescriptorSets(uiElement.model, MAX_FRAMES_IN_FLIGHT);
}
void AetherEngine::loadUIElements() {
	uiMap[uiElementId::MainMenu] = createUIElement("qml/MainMenu.qml", windowWidth, windowHeight, mainWindow);
	uiMap[uiElementId::SettingsMenu] = createUIElement("qml/SettingsMenu.qml", windowWidth, windowHeight, mainWindow);
	uiMap[uiElementId::PauseMenu] = createUIElement("qml/PauseMenu.qml", windowWidth, windowHeight, mainWindow);
	uiMap[uiElementId::InGameOverlay] = createUIElement("qml/InGameOverlay.qml", windowWidth, windowHeight, mainWindow);
	uiMap[uiElementId::SelectEquation] = createUIElement("qml/SelectEquation.qml", windowWidth, windowHeight, mainWindow);
	uiMap[uiElementId::SolveEquation] = createUIElement("qml/SolveEquation.qml", windowWidth, windowHeight, mainWindow);

	initializeInGameOverlayCache();

	inGameWindow->setMainMenuRenderer(uiMap[uiElementId::MainMenu].renderer.get());
	inGameWindow->setSettingsMenuRenderer(uiMap[uiElementId::SettingsMenu].renderer.get());
	inGameWindow->setPauseMenuRenderer(uiMap[uiElementId::PauseMenu].renderer.get());
	inGameWindow->setSelectEquationRenderer(uiMap[uiElementId::SelectEquation].renderer.get());
	inGameWindow->setSolveEquationRenderer(uiMap[uiElementId::SolveEquation].renderer.get());

	MainMenuSlotHandler* mainMenuSlotHandler = new MainMenuSlotHandler(this);
	SettingsMenuSlotHandler* settingsMenuSlotHandler = new SettingsMenuSlotHandler(this);
	PauseMenuSlotHandler* pauseMenuSlotHandler = new PauseMenuSlotHandler(this);
	SelectEquationSlotHandler* selectEquationSlotHandler = new SelectEquationSlotHandler(this);
	SolveEquationSlotHandler* solveEquationSlotHandler = new SolveEquationSlotHandler(isSolveEquationTextFieldActivated, this);

	auto rootItem = uiMap[uiElementId::MainMenu].renderer->getRootItem();
	connect(rootItem, SIGNAL(startGameClicked()), mainMenuSlotHandler, SLOT(onStartGameClicked()));
	connect(rootItem, SIGNAL(openSettingsClicked()), mainMenuSlotHandler, SLOT(onOpenSettingsClicked()));
	connect(rootItem, SIGNAL(exitGameClicked()), mainMenuSlotHandler, SLOT(onExitGameClicked()));

	rootItem = uiMap[uiElementId::SettingsMenu].renderer->getRootItem();
	connect(rootItem, SIGNAL(returnToMainMenuClicked()), settingsMenuSlotHandler, SLOT(onReturnToMainMenuClicked()));

	rootItem = uiMap[uiElementId::PauseMenu].renderer->getRootItem();
	connect(rootItem, SIGNAL(resumeGameClicked()), pauseMenuSlotHandler, SLOT(onResumeGameClicked()));
	connect(rootItem, SIGNAL(openSettingsClicked()), pauseMenuSlotHandler, SLOT(onOpenSettingsClicked()));
	connect(rootItem, SIGNAL(openMainMenuClicked()), pauseMenuSlotHandler, SLOT(onOpenMainMenuClicked()));
	connect(rootItem, SIGNAL(exitGameClicked()), pauseMenuSlotHandler, SLOT(onExitGameClicked()));

	rootItem = uiMap[uiElementId::SelectEquation].renderer->getRootItem();
	connect(
		rootItem, SIGNAL(buttonClicked(int)), 
		selectEquationSlotHandler, SLOT(onButtonClicked(int))
	);

	rootItem = uiMap[uiElementId::SolveEquation].renderer->getRootItem();
	connect(
		rootItem, SIGNAL(answerSubmitted(QString)), 
		solveEquationSlotHandler, SLOT(onAnswerSubmitted(QString))
	);
}
void AetherEngine::initializeInGameOverlayCache() {
	auto& uiElement = uiMap[uiElementId::InGameOverlay];

	auto renderer = uiElement.renderer.get();
	if (!renderer) return;
	auto rootItem = renderer->getRootItem();
	if (!rootItem) return;

	auto cache = std::make_unique<InGameOverlayCache>();
	cache->rootItem = rootItem;

	cache->minimapContainer = rootItem->findChild<QObject*>("minimapContainer");

	cache->playerHealth = rootItem->findChild<QObject*>("playerHealth");
	cache->playerDamage = rootItem->findChild<QObject*>("playerDamage");
	cache->playerDefense = rootItem->findChild<QObject*>("playerDefense");
	cache->playerExperience = rootItem->findChild<QObject*>("playerExperience");

	cache->mobTitle = rootItem->findChild<QObject*>("mobTitle");
	cache->mobHealth = rootItem->findChild<QObject*>("mobHealth");
	cache->mobDamage = rootItem->findChild<QObject*>("mobDamage");
	cache->mobDefense = rootItem->findChild<QObject*>("mobDefense");
	cache->mobExperienceReward = rootItem->findChild<QObject*>("mobExperience");

	uiElement.cache = std::move(cache);
}

void AetherEngine::createMainWindow()
{
	mainWindow = new MainWindow();
	mainWindow->resize(windowWidth, windowHeight);
	//mainWindow->setWindowState(Qt::WindowFullScreen);
    //mainWindow->setWindowFlags(Qt::FramelessWindowHint);
	mainWindow->setWindowTitle("Math dungeon");
	mainWindow->setWindowIcon(QIcon("textures/granite_square.png"));
	//mainWindow->setWindowState(Qt::WindowMaximized);

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
	
	inGameWindow = new InGameWindow(&qVulkanInstance, character);
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
	case GameState::COMBAT_PLAYER_SELECT_EQUATION:
		isSelectEquationActivated = false;
		break;
	case GameState::COMBAT_PLAYER_SOLVE_EQUATION:
		isSolveEquationTextFieldActivated = false;
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
		stackedWidget->setCurrentWidget(inGameWidget);
		break;
	case GameState::SETTINGS_MENU:
		stackedWidget->setCurrentWidget(inGameWidget);
		break;
	case GameState::IN_GAME_TESTING:
		QApplication::setOverrideCursor(Qt::BlankCursor);
		character.camera._isFirstMouse = true;
		stackedWidget->setCurrentWidget(inGameWidget);
		//if (!inGameWidget->hasFocus()) { inGameWidget->setFocus(); }
		break;
	case GameState::DUNGEON_EXPLORATION:
		stackedWidget->setCurrentWidget(inGameWidget);
		if (!inGameWindow->isActive()) { inGameWindow->requestActivate(); }
		//if (!inGameWidget->hasFocus()) { inGameWidget->setFocus(); }
		//if (!inGameWindow->isActive()) { inGameWindow->requestActivate(); }
		break;
	case GameState::COMBAT_PLAYER_SELECT_EQUATION:
		stackedWidget->setCurrentWidget(inGameWidget);
		updateSelectEquation(3);
		//if (!inGameWidget->hasFocus()) { inGameWidget->setFocus(); }
		break;
	case GameState::COMBAT_PLAYER_SOLVE_EQUATION:
		stackedWidget->setCurrentWidget(inGameWidget);
		clearSolveEquationInput();
		gameContext.timeRemainingToSolveEquation = gameContext.selectedEquation->timeToSolve;
		//if (!inGameWidget->hasFocus()) { inGameWidget->setFocus(); }
		break;
	case GameState::PAUSED:
		stackedWidget->setCurrentWidget(inGameWidget);
		if (!inGameWindow->isActive()) { inGameWindow->requestActivate(); }
		break;
	case GameState::EXIT:
		QCoreApplication::quit();
		break;
	default:
		break;
	}
}
void AetherEngine::handleInGameTestingState(double deltaTime, double timeSinceLaunch)
{
	character.handleInGamePlayerInput();
	if (!character.isInteracting) {
		character.handleCharacterMovement(deltaTime, gravity, models);
	}

	// skipping qt mouse movements on the first frame
	character.camera.handleRotation(inGameWindow->latestMouseDx, inGameWindow->latestMouseDy);

	if (inGameWindow->isActive()) {
		QCursor::setPos(gameContext.windowCenterPos);
		//std::cout << "set mouse pos 0: " << gameContext.windowCenterPos.x() << " " << gameContext.windowCenterPos.y() << "\n";
	}
}

void AetherEngine::handleRightAnswer(Equation& selectedEquation) {
	auto& mobs = gameContext.currentRoom->mobs;
	Mob& mob = mobs[0];
	mob.takeDamage(selectedEquation.damage + character.attackPower);

	// Award experience for dead mobs BEFORE erasing them
	for (const Mob& mob : mobs) {
		if (!mob.isAlive()) {
			character.experience += mob.experienceReward;
		}
	}

	mobs.erase(
		std::remove_if(mobs.begin(), mobs.end(), [](const Mob& mob) { return !mob.isAlive(); }), 
		mobs.end()
	);

	if (gameContext.currentRoom->hasMobs()) {
		gameContext.requestedGameState = GameState::COMBAT_MOB_TURN;
		//gameContext.requestedGameState = GameState::COMBAT_PLAYER_SELECT_EQUATION;
	}
	else {
		gameContext.requestedGameState = GameState::DUNGEON_ROOM_CLEANED;
	}

	gameContext.submittedAnswer = std::numeric_limits<double>::max();
	selectedEquation.isSolved = true;
}
void AetherEngine::handleWrongAnswer(const Equation& selectedEquation) {
	gameContext.timeRemainingToSolveEquation -= selectedEquation.wrongAnswerPenalty;
	gameContext.timeRemainingToSolveEquation = std::max(gameContext.timeRemainingToSolveEquation, 0.0);
}
void AetherEngine::handleEquationSolving() {
	if (gameContext.timeRemainingToSolveEquation <= 0.0) {
		gameContext.requestedGameState = GameState::COMBAT_MOB_TURN;
		gameContext.isAnswerSubmitted = false;
		return;
	}

	if (!gameContext.keyboardKeys[Qt::Key_Enter]) {
		gameContext.answerSubmissionAllowed = true;
	}
	/*else if (gameContext.keyboardKeys[Qt::Key_Enter]) {
		std::cout << "no\n";
	}*/
	if (gameContext.answerSubmissionAllowed && gameContext.isAnswerSubmitted) {
		Equation& selectedEquation = *gameContext.selectedEquation;
		if (std::abs(gameContext.submittedAnswer - selectedEquation.answer) < 0.25) {
			handleRightAnswer(selectedEquation);
		}
		else {
			handleWrongAnswer(selectedEquation);
		}

		gameContext.answerSubmissionAllowed = false;
		gameContext.isAnswerSubmitted = false;
	}
}

void AetherEngine::mainLoop()
{
	bool fpsMenu = 0;
	uint32_t counter = 0;
	double accumulator = 0.0;
	double fps = 0.0;

	double deltaTime = 0.0;
	double timeSinceLaunch = 0.0;
	std::chrono::high_resolution_clock::time_point currentTime;
	std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();

	while (gameContext.currentGameState != GameState::EXIT) {
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - previousTime).count();
		previousTime = currentTime;
		timeSinceLaunch += deltaTime;

		QCoreApplication::processEvents();

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

		if (gameContext.keyboardKeys[Qt::Key_R]) {
			//loadUIElements();
			gameContext.requestedGameState = GameState::PLAYER_DEAD;
		}

		//std::cout << "inGameWindow->isActive(): " << inGameWindow->isActive() << "\n";

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

		//std::cout << "state: " << static_cast<uint32_t>(gameContext.currentGameState) << "\n";
		//std::cout << "is active: " << mainWindow->hasFocus() << "\n";
		//mainWindow->setFocus();
		//std::cout << "is exposed: " << mainWindow->windowHandle()->isExposed() << "\n";

		if (!inGameWindow->isExposed()) { continue; }

		if (gameContext.currentGameState == GameState::IN_GAME_TESTING) {
			handleInGameTestingState(deltaTime, timeSinceLaunch);
		}
		if (gameContext.currentGameState == GameState::DUNGEON_EXPLORATION) {
			if (gameContext.currentRoom->hasMobs()) {
				gameContext.requestedGameState = GameState::COMBAT_PLAYER_SELECT_EQUATION;
			}

			character.handleDungeonExplorationPlayerInput();
		}
		if (gameContext.currentGameState == GameState::COMBAT_PLAYER_SELECT_EQUATION) {
			if (!isSelectEquationActivated) {
				inGameWindow->requestActivate();
				isSelectEquationActivated = true;
			}

			if (gameContext.keyboardKeys[Qt::Key_Escape]) {
				//gameContext.requestedGameState = GameState::DUNGEON_EXPLORATION;
			}

			for (size_t i = 0; i < 3; ++i) {
				if (gameContext.keyboardKeys[Qt::Key_1 + i]) {
					gameContext.selectedEquation = &gameContext.equations[i];
					gameContext.requestedGameState = GameState::COMBAT_PLAYER_SOLVE_EQUATION;
					break;
				}
			}
		}
		if (gameContext.currentGameState == GameState::COMBAT_PLAYER_SOLVE_EQUATION) {
			gameContext.timeRemainingToSolveEquation = 
				std::max(gameContext.timeRemainingToSolveEquation - deltaTime, 0.0);

			updateSolveEquationOverlay();

			handleEquationSolving();
		}
		if (gameContext.currentGameState == GameState::COMBAT_MOB_TURN) {
			character.takeDamage(
				gameContext.currentRoom->mobs[0].attackPower - 
				(gameContext.selectedEquation->isSolved ? 
				gameContext.selectedEquation->defence : 
				0)
			);

			if (character.isAlive()) {
				gameContext.requestedGameState = GameState::COMBAT_PLAYER_SELECT_EQUATION;
			}
			else {
				gameContext.requestedGameState = GameState::PLAYER_DEAD;
			}
		}
		if (gameContext.currentGameState == GameState::DUNGEON_ROOM_CLEANED) {
			if (Dungeon::isDungeonFloorClear(gameContext.dungeonFloor)) {
				// loops from 0 to 2, than again 0
				gameContext.currentFloor = (gameContext.currentFloor + 1) % 3;

				recreateDungeonFloor(gameContext.currentFloor, 1);
				gameContext.currentRoom = character.enterDungeonFloor(gameContext.dungeonFloor);
			}

			gameContext.requestedGameState = GameState::DUNGEON_EXPLORATION;
		}
		if (gameContext.currentGameState == GameState::PLAYER_DEAD) {
			character.health = character.maxHealth;

			gameContext.currentFloor = 0;
			recreateDungeonFloor(gameContext.currentFloor, 1);
			gameContext.currentRoom = character.enterDungeonFloor(gameContext.dungeonFloor);

			gameContext.requestedGameState = GameState::DUNGEON_EXPLORATION;
		}
		if (gameContext.currentGameState == GameState::MAIN_MENU					 ||
			gameContext.currentGameState == GameState::SETTINGS_MENU				 ||
			gameContext.currentGameState == GameState::DUNGEON_EXPLORATION			 ||
			gameContext.currentGameState == GameState::IN_GAME_TESTING				 ||
			gameContext.currentGameState == GameState::COMBAT_PLAYER_SELECT_EQUATION ||
			gameContext.currentGameState == GameState::COMBAT_PLAYER_SOLVE_EQUATION  ||
			gameContext.currentGameState == GameState::COMBAT_MOB_TURN				 ||
			gameContext.currentGameState == GameState::DUNGEON_ROOM_CLEANED          ||
			gameContext.currentGameState == GameState::PAUSED) {

			if (gameContext.isCameraTransitioning) {
				gameContext.cameraCurrentTransitionTime += deltaTime;

				float rawT = std::min(gameContext.cameraCurrentTransitionTime / gameContext.cameraTransitionDuration, 1.0f);
				float t = std::min(rawT * rawT * (3.0f - 2.0f * rawT), 1.0f); // smoothstep easing
				glm::vec3 newPos = glm::mix(gameContext.cameraStartPosition, gameContext.cameraTargetPosition, t);
				character.camera.setPosition(newPos);

				if (t >= 1.0f) {
					Dungeon::updateRoomsState(
						*gameContext.targetRoom, 
						*gameContext.currentRoom, 
						gameContext.dungeonFloor.dungeonRooms, 2
					);

					gameContext.currentRoom = gameContext.targetRoom;
					gameContext.targetRoom = nullptr;
					gameContext.isCameraTransitioning = false;
				}
			}

			updateInGameOverlay();

			drawFrame(timeSinceLaunch, deltaTime);
		}
	}

	vkDeviceWaitIdle(vkInit.device);
}
void AetherEngine::recreateDungeonFloor(int32_t floorNumber, float difficultyScale)
{
	// clear only dungeon models
	vkDeviceWaitIdle(vkInit.device);
	cleanupModelsByType(models, { ModelType::DUNGEON });

	// reset the dungeon floor
	gameContext.dungeonFloor = {};

	//if (dungeonTextures.empty()) {
		modelManager.loadTexturesFromFolder("textures/dungeon", dungeonTextures);
	//}
	/*std::chrono::high_resolution_clock::time_point currentTime;
	float deltaTime;
	std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();

	currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - previousTime).count();
	if (deltaTime > 1e-9) {
		std::cout << "time to create: " << deltaTime << "\n";
	}*/

	std::string wallKey = "stone_wall_floor_" + std::to_string(floorNumber);
	std::string floorKey = "stone_floor_floor_" + std::to_string(floorNumber);

	Texture wallTexture = dungeonTextures.contains(wallKey) ? dungeonTextures[wallKey] : notFoundTexture;
	Texture floorTexture = dungeonTextures.contains(floorKey) ? dungeonTextures[floorKey] : notFoundTexture;

	std::vector<Model> dungeonModels = Dungeon::generateRandomDungeonFloor(
		floorNumber, difficultyScale,
		gameContext.dungeonFloor,
		floorTexture,
		wallTexture
	);
	bufferManager.computeAABB_createVertexIndexBuffers(dungeonModels);
	bufferManager.createShaderBuffers(dungeonModels, MAX_FRAMES_IN_FLIGHT);
	createDescriptorSets(dungeonModels, MAX_FRAMES_IN_FLIGHT);

	models.insert(models.end(), dungeonModels.begin(), dungeonModels.end());
}

void AetherEngine::cleanupMemory()
{
	cleanupModel(sky);
	cleanupModels(models);

	for (auto& [id, elem] : uiMap) {
		cleanupModel(elem.model);
	}

	cleanupTexture(grassTexture);
	cleanupTexture(floor_background);
	cleanupTexture(transparentTexture);
	cleanupTexture(notFoundTexture);
	cleanupTexture(depthTexture);
	cleanupTexture(msaaTexture);

	cleanupSwapchain();

	deletedTextureHashes.clear();

	if (vkInit.enableValidationLayers) {
		std::cout << "createdVmaAllocations:   " << gameContext.createdVmaAllocations << "\n";
		std::cout << "destroyedVmaAllocations: " << gameContext.destroyedVmaAllocations << "\n";
	}

	// will free all allocated descriptor sets from this pool
	vkDestroyDescriptorPool(vkInit.device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(vkInit.device, descriptorSetLayout, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(vkInit.device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(vkInit.device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(vkInit.device, inFlightFences[i], nullptr);
	}
	
	vmaDestroyAllocator(vkInit.vmaAllocator);
	vkDestroyCommandPool(vkInit.device, vkInit.commandPool, nullptr);

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
void AetherEngine::cleanupModelsByType(
	std::vector<Model>& models, const std::unordered_set<ModelType>& types
) const {
	models.erase(std::remove_if(models.begin(), models.end(),
		[this, &types](Model& model) {
			if (types.count(model.type)) {
				cleanupModel(model);
				return true;
			}
			return false;
		}), models.end()
	);
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
		vmaDestroyBuffer(vkInit.vmaAllocator, mesh.vertexBuffer, mesh.vertexBufferAllocation);
		mesh.vertexBuffer = VK_NULL_HANDLE;
		mesh.vertexBufferAllocation = VK_NULL_HANDLE;
		gameContext.destroyedVmaAllocations += 1;
	}

	if (mesh.indexBufferAllocation != VK_NULL_HANDLE) {
		vmaDestroyBuffer(vkInit.vmaAllocator, mesh.indexBuffer, mesh.indexBufferAllocation);
		mesh.indexBuffer = VK_NULL_HANDLE;
		mesh.indexBufferAllocation = VK_NULL_HANDLE;
		gameContext.destroyedVmaAllocations += 1;
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
	if (!texture || texture.hash == 0) {
		return;
	}

	if (deletedTextureHashes.insert(texture.hash).second) {
		if (texture.imageView) {
			vkDestroyImageView(vkInit.device, texture.imageView, nullptr);
			texture.imageView = VK_NULL_HANDLE;
		}
		if (texture.sampler) {
			vkDestroySampler(vkInit.device, texture.sampler, nullptr);
			texture.sampler = VK_NULL_HANDLE;
		}
		if (texture.image || texture.vmaAllocation) {
			vmaDestroyImage(vkInit.vmaAllocator, texture.image, texture.vmaAllocation);
			texture.image = VK_NULL_HANDLE;
			texture.vmaAllocation = VK_NULL_HANDLE;
			gameContext.destroyedVmaAllocations += 1;
		}

		texture.width = 0;
		texture.height = 0;
		texture.mipLevels = 0;
		texture.hash = 0;
	}
	/*else {
		std::cout << "texture with this hash has already been deleted: " << texture.hash << "\n";
	}*/
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
			vmaDestroyBuffer(vkInit.vmaAllocator, mesh.UBOBuffers[i], mesh.UBOAllocations[i]);
			mesh.UBOBuffers[i] = VK_NULL_HANDLE;
			mesh.UBOAllocations[i] = VK_NULL_HANDLE;
			gameContext.destroyedVmaAllocations += 1;
		}

		if (mesh.boneSSBOAllocations[i] != VK_NULL_HANDLE) {
			vmaDestroyBuffer(vkInit.vmaAllocator, mesh.boneSSBOBuffers[i], mesh.boneSSBOAllocations[i]);
			mesh.boneSSBOBuffers[i] = VK_NULL_HANDLE;
			mesh.boneSSBOAllocations[i] = VK_NULL_HANDLE;
			gameContext.destroyedVmaAllocations += 1;
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

// old method
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

// creating fences and semaphores
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

// wrapping shader in shader module
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