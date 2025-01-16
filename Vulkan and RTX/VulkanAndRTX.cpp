#include "pch.h"
#include "VulkanAndRTX.h"
#include "TerrainGenerator.h"

static ImGui_ImplVulkanH_Window g_MainWindowData;

void VulkanAndRTX::run()
{
	createGLFWWindow();
	character.initializeInputHandler(window);
	vkInit.initializeVulkan(window);
	prepareResources();
	vulkanWindow = &g_MainWindowData;
	setupImguiWindow(vulkanWindow, vkInit.surface, windowWidth, windowHeight);
	setupImGui();
	initializeNoesisGUI();
	mainLoop();
	cleanupMemory();
}

// initializing GLFW and creating window
void VulkanAndRTX::createGLFWWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

	window = glfwCreateWindow(windowWidth, windowHeight, "Vulkan and RTX", nullptr, nullptr);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	std::cout << "monitor count: " << count << "\n";
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	std::cout << "current monitor mode: " << mode->redBits << " " << mode->greenBits << " " << mode->blueBits
		<< " " << mode->width << " " << mode->height << " " << mode->refreshRate << "\n";

	// setting window to the center
	glfwSetWindowPos(window, mode->width / 2 - windowWidth / 2, mode->height / 2 - windowHeight / 2 - 30);

	/*int width_mm, height_mm;
	glfwGetMonitorPhysicalSize(monitor, &width_mm, &height_mm);
	std::cout << "monitor width, height: " << width_mm << " " << height_mm << "\n";
	const char* name = glfwGetMonitorName(monitor);
	std::cout << "monitor name: " << name << "\n";

	int count1;
	const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count1);

	for (int i = 0; i < count1; i++)
	{
		std::cout << "video mode: " << i << " " << modes[i].redBits << " " << modes[i].greenBits << " " << modes[i].blueBits << " "
			<< modes[i].width << " " << modes[i].height << " " << modes[i].refreshRate << "\n";
	}*/

	// Icon creation
#pragma region
	int width, height;
	int channels;
	unsigned char* pixels = stbi_load("textures/icon.png", &width, &height, &channels, 4);

	GLFWimage windowIcon[1]{};
	windowIcon[0].height = height;
	windowIcon[0].width = width;
	windowIcon[0].pixels = pixels;

	glfwSetWindowIcon(window, 1, windowIcon);
#pragma endregion
}
void VulkanAndRTX::initializeNoesisGUI()
{
	Noesis::GUI::Init();

#ifdef NDEBUG
	Noesis::SetLogHandler(
		[](const char* file, uint32_t line, uint32_t level, const char* channel, const char* message) {
			printf("[%s:%u] %s: %s\n", file, line, channel, message);
		}
	);
	Noesis::SetErrorHandler(
		[](const char* file, uint32_t line, const char* message, bool fatal) {
		printf("[%s:%u] %s: %s\n", file, line, message, fatal ? "true" : "false");
		}
	);
#endif

	std::string xamlRootPath = std::filesystem::current_path().string();
	Noesis::Ptr<NoesisApp::LocalXamlProvider> xamlProvider = *new NoesisApp::LocalXamlProvider(xamlRootPath.c_str());
	Noesis::GUI::SetXamlProvider(xamlProvider);

	NoesisApp::VKFactory::InstanceInfo instanceInfo{};
	instanceInfo.instance = vkInit.instance;
	instanceInfo.physicalDevice = vkInit.physicalDevice;
	instanceInfo.device = vkInit.device;
	instanceInfo.pipelineCache = VK_NULL_HANDLE; // Optional pipeline cache
	instanceInfo.queueFamilyIndex = vkInit.queueFamilyIndices.presentFamily.value();
	instanceInfo.vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
		vkGetInstanceProcAddr(vkInit.instance, "vkGetInstanceProcAddr")
	);

    // Initialize NoesisGUI with a Vulkan render device
	noesisRenderDevice = NoesisApp::VKFactory::CreateDevice(true, instanceInfo);
	if (!noesisRenderDevice) {
		throw std::runtime_error("Failed to create Noesis Vulkan render device");
	}

	NoesisApp::VKFactory::SetRenderPass(noesisRenderDevice, objectRenderPass, vkInit.colorSamples);

    // Load your XAML file
    Noesis::Ptr<Noesis::FrameworkElement> xamlElement = Noesis::GUI::LoadXaml
		<Noesis::FrameworkElement>(Noesis::Uri("UI/MainMenu.xaml"));
	if (!xamlElement) {
		throw std::runtime_error("Failed to load XAML file");
	}
	
	// Create a renderer for the XAML element
	noesisView = Noesis::GUI::CreateView(xamlElement);
	if (!noesisView) {
		throw std::runtime_error("Failed to create Noesis renderer");
	}

	noesisView->SetSize(swapChainExtent.width, swapChainExtent.height);

    // Attach the render device to the renderer
	noesisView->GetRenderer()->Init(noesisRenderDevice);
}

void VulkanAndRTX::setupImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForVulkan(window, true);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = vkInit.instance;
	init_info.PhysicalDevice = vkInit.physicalDevice;
	init_info.Device = vkInit.device;
	init_info.QueueFamily = vkInit.queueFamilyIndices.graphicsFamily.value();
	init_info.Queue = vkInit.graphicsQueue;
	init_info.PipelineCache = nullptr;
	init_info.DescriptorPool = descriptorPool;
	init_info.RenderPass = objectRenderPass;
	init_info.Subpass = 0;
	init_info.MinImageCount = MAX_FRAMES_IN_FLIGHT;
	init_info.ImageCount = vulkanWindow->ImageCount;
	init_info.MSAASamples = vkInit.colorSamples;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = [](VkResult err) {
		if (err != VK_SUCCESS) {
			std::cerr << "Vulkan error: " << err << std::endl;
		}
	};

	ImGui_ImplVulkan_Init(&init_info);
}
void VulkanAndRTX::setupImguiWindow(ImGui_ImplVulkanH_Window* wd, 
	VkSurfaceKHR surface, size_t width, size_t height)
{
	wd->Surface = surface;

	// Check for WSI support
	VkBool32 res;
	vkGetPhysicalDeviceSurfaceSupportKHR(vkInit.physicalDevice, 
		vkInit.queueFamilyIndices.graphicsFamily.value(), wd->Surface, &res);
	if (res != VK_TRUE)
	{
		fprintf(stderr, "Error no WSI support on physical device 0\n");
		exit(-1);
	}

	// Select Surface Format
	const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
	const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(vkInit.physicalDevice, wd->Surface, 
		requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), 
		requestSurfaceColorSpace);

	// Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
	VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
	VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
	wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(vkInit.physicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
	//printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

	// Create SwapChain, RenderPass, Framebuffer, etc.
#ifdef NDEBUG
	IM_ASSERT(g_MinImageCount >= 2);
#endif // !NDEBUG

	ImGui_ImplVulkanH_CreateOrResizeWindow(vkInit.instance, vkInit.physicalDevice, vkInit.device, wd, 
		vkInit.queueFamilyIndices.graphicsFamily.value(), nullptr,
		width, height, MAX_FRAMES_IN_FLIGHT);
}

// set "framebufferResized" to "true" if window was resized or moved
void VulkanAndRTX::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<VulkanAndRTX*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
	character.camera.setViewportSize(width, height);
	noesisView->SetSize(width, height);
}

void VulkanAndRTX::prepareResources()
{
	//std::chrono::high_resolution_clock::time_point currentTime;
	//float deltaTime;
	//std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
	
	//currentTime = std::chrono::high_resolution_clock::now();
	//deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - previousTime).count();
	//std::cout << "time to create gpp: " << deltaTime << "\n";
	
	createSwapChain();
	createSwapChainImageViews();
	createObjectRenderPass(objectRenderPass);
	createGUIRenderPass(noesisRenderPass);
	createDescriptorSetLayout(descriptorSetLayout);
	createGraphicsPipeline(
		PIPELINE_TYPE_OBJECT,
		"object", "shaders/object.vert.spv", "shaders/object.frag.spv",
		objectRenderPass
	);
	createGraphicsPipeline(
		PIPELINE_TYPE_SKY,
		"sky", "shaders/sky.vert.spv", "shaders/sky.frag.spv",
		objectRenderPass
	);
	createGraphicsPipeline(
		PIPELINE_TYPE_GUI,
		"noesis", "shaders/noesis.vert.spv", "shaders/noesis.frag.spv",
		noesisRenderPass
	);
	createCommandPool();
	createColorTexture(msaaTexture);
	createDepthTexture(depthTexture);
	createSwapChainFramebuffers();

	createTextureFromPath("textures/grass001.png", grassTexture);
	createDummyTexture({ 0, 0, 0, 255 }, dummyTexture);

	TerrainData terrainData = {
		100, 100, // chunkWidth, chunkLength
		4, 4,     // chunkRows, chunkCols
		2.0f,     // gridSize
		0.1f,     // scale
		1.0f,     // height
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

	// loadModelsFromDirectory("models", models);

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

	createSkyUniformBuffers(MAX_FRAMES_IN_FLIGHT);
	createMeshShaderBuffers(MAX_FRAMES_IN_FLIGHT);
	createSkyDescriptorSets(MAX_FRAMES_IN_FLIGHT);
	createMeshDescriptorSets(MAX_FRAMES_IN_FLIGHT);

	createCommandBuffers();
	createSyncObjects();
}

void VulkanAndRTX::mainLoop()
{
	std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point currentTime;
	double deltaTime;
	double timeSinceLaunch = 0.0f;
	
	int counter = 0;
	float accumulator = 0;
	float fps = 0;
	bool fpsMenu = false;
	while (!glfwWindowShouldClose(window)) {
		// accurate time measurement for synchronization
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - previousTime).count();
		previousTime = currentTime;
		timeSinceLaunch += deltaTime;

		glfwPollEvents();
		if (!character.currentInteractingVolume) {
			character.handleKeyInput(
				deltaTime, 
				gravity, models);
		}
		//restrictCharacterMovement(character.camera);

		// Start the ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		// fps meter
		{
			counter++;
			accumulator += deltaTime;
			if (accumulator >= 1.1)
			{
				fps = 1 / (accumulator / counter);
				counter = 0;
				accumulator = 0;
			}

			ImGui::Begin("fps", &fpsMenu, ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_NoBackground);
			ImGui::SetWindowSize(ImVec2(100, 30));
			ImGui::SetWindowPos(ImVec2(10, 10));
			//ImGui::SetWindowFontScale(1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
			ImGui::Text("%.2f", fps);
			ImGui::PopStyleColor(); // Pop color style
			ImGui::End();
		}
		
		// interaction menu
		{
			if (character.currentInteractingVolume && character.currentInteractingVolume->isOpen) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				//glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

				bool menuOpen = character.currentInteractingVolume->isOpen;
				ImGui::Begin((&character.currentInteractingVolume->name)->c_str(), &menuOpen);

				ImGui::Text("Solve the equation");

				if (!puzzleGenerated) {
					puzzleEquation = createPuzzleEquation(
						character.currentInteractingVolume->name, puzzleAnswer);
					std::cout << puzzleAnswer << "\n";
					puzzleGenerated = true;
					timeToSolvePuzzle = 6.5f;
				}
				//ImGui::Text((char*)&std::to_string(puzzleAnswer));
				std::string timeStr = std::to_string(timeToSolvePuzzle);
				ImGui::Text(timeStr.c_str());
				ImGui::Text((char*)&puzzleEquation);
				ImGui::InputInt("Enter the answer: ", &puzzleInput);

				if (puzzleInput == puzzleAnswer) {
					puzzleInput = 0;
					puzzleGenerated = false;
				}

				//std::cout << inputNumber << "\n";
				//menuOpen = false;
				if (!menuOpen || timeToSolvePuzzle <= 0.0f) {
					character.currentInteractingVolume->isOpen = menuOpen;
					character.currentInteractingVolume = nullptr;
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					puzzleInput = 0;
					puzzleGenerated = false;

					//glfwSetCursorPos(window, lastMousePosX / 2, lastMousePosY / 2);
				}
				timeToSolvePuzzle -= deltaTime;

				ImGui::End();
			}
		}

		ImGui::Render();
		ImDrawData* draw_data = ImGui::GetDrawData();
		drawFrame(timeSinceLaunch, deltaTime, draw_data);
	}

	vkDeviceWaitIdle(vkInit.device);
}

void VulkanAndRTX::cleanupImGui() {
	VkResult err = vkDeviceWaitIdle(vkInit.device);
    check_vk_result(err);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void VulkanAndRTX::cleanupModels()
{
	for (size_t i = 0; i < models.size(); i++) {
		cleanupModel(models[i]);
	}

	cleanupModel(sky);
}
void VulkanAndRTX::cleanupModel(Model& model) const
{
	for (size_t i = 0; i < model.meshes.size(); i++) {
		vkDestroyBuffer(vkInit.device, model.meshes[i].indexBuffer, nullptr);
		vkFreeMemory(vkInit.device, model.meshes[i].indexBufferMemory, nullptr);

		vkDestroyBuffer(vkInit.device, model.meshes[i].vertexBuffer, nullptr);
		vkFreeMemory(vkInit.device, model.meshes[i].vertexBufferMemory, nullptr);
	}
	for (size_t i = 0; i < model.materials.size(); i++)
	{
		// cleanupMaterial(model.materials[i]);
	}
}
void VulkanAndRTX::cleanupMaterial(Material& material) const
{
	cleanupTexture(material.diffuseTexture);
	cleanupTexture(material.normalTexture);
	cleanupTexture(material.specularTexture);
	cleanupTexture(material.emissiveTexture);
}
void VulkanAndRTX::cleanupTexture(Texture& texture) const
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

void VulkanAndRTX::cleanupMemory()
{
	/*if (noesisRenderer) {
		noesisRenderer->Shutdown();
		noesisRenderer.Reset();
	}*/
	if (noesisView) {
		noesisView.Reset();
	}
	if (noesisRenderDevice) {
		noesisRenderDevice.Reset();
	}
	Noesis::GUI::Shutdown();

	cleanupImGui();

	cleanupModels();
	cleanupTexture(grassTexture);
	cleanupTexture(dummyTexture);

	cleanupSwapChain();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(vkInit.device, skyUniformBuffers[i], nullptr);
		vkFreeMemory(vkInit.device, skyUniformBuffersMemory[i], nullptr);
	}

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
	vkDestroyInstance(vkInit.instance, nullptr);

	glfwDestroyWindow(window);

	glfwTerminate();
}
void VulkanAndRTX::check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

void VulkanAndRTX::restrictCharacterMovement(Camera& camera)
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

std::string VulkanAndRTX::createPuzzleEquation(std::string name, int& answer)
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
std::vector<char> VulkanAndRTX::readFile(const std::string& filename)
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
uint32_t VulkanAndRTX::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
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
void VulkanAndRTX::createCommandPool()
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
void VulkanAndRTX::createSyncObjects()
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
VkShaderModule VulkanAndRTX::createShaderModule(const std::vector<char>& code) const
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

static int runVulkanAndRTX() {
	srand(static_cast<unsigned>(time(0))); // Seed the random number generator once
	VulkanAndRTX app;
	try {
		app.run();
		return EXIT_SUCCESS;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}

int main() {
	return runVulkanAndRTX();
}