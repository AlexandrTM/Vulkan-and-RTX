#include "pch.h"
#include "VulkanAndRTX.h"

void VulkanAndRTX::run()
{
	createWindow();
	inputHandler.initializeInputHandler(window);
	vkInit.initializeVulkan(window);
	prepareResources();
	mainLoop();
	cleanupMemory();
}

// initializing GLFW and creating window
void VulkanAndRTX::createWindow()
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

// set "framebufferResized" to "true" if window was resized or moved
void VulkanAndRTX::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<VulkanAndRTX*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
	inputHandler.camera.setViewportSize(width, height);
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
	createRenderPass();
	createDescriptorSetLayout(descriptorSetLayout);
	createGraphicsPipeline("object", "shaders/object.vert.spv", "shaders/object.frag.spv");
	createGraphicsPipeline("sky", "shaders/sky.vert.spv", "shaders/sky.frag.spv");
	createCommandPool();
	createColorResources();
	createDepthResources();
	createSwapChainFramebuffers();

	createTextureImage("textures/grass001.png", textureImage, textureImageMemory);
	createTextureImageView(textureImage, textureImageView);
	createTextureSampler(textureSampler);

	//loadGltfModel("models/blue_archivekasumizawa_miyu.glb");
	generateTerrain(-300, -300, 600, 600, 1.0, 0.2, 1);
	generateCuboid(20.0, 0.0 , -10.0, 
				   1.75, 4.75,  1.75, glm::vec3(0.0, 0.7, 0.0));
	generateCuboid(20.0, 0.0 ,   0.0,
				   1.75, 4.75,  1.75, glm::vec3(0.52, 0.52, 0.0));
	generateCuboid(20.0, 0.0 ,  10.0,
				   1.75, 4.75,  1.75, glm::vec3(0.7, 0.0, 0.0));

	generateSkyCube();

	createVertexBuffer(models.sky);
	createIndexBuffer(models.sky);

	for (size_t i = 0; i < models.objects.size(); i++) {
		createVertexBuffer(models.objects[i]);
		createIndexBuffer(models.objects[i]);
	}
	//std::cout << "models:" << models.size() << "\n";

	createUniformBuffers();

	createDescriptorPool();
	createDescriptorSets();

	createCommandBuffers();
	createSyncObjects();
}

void VulkanAndRTX::mainLoop()
{
	std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point currentTime;
	float deltaTime;
	float timeSinceLaunch = 0.0f;

	//int counter = 0;
	//float accumulator = 0;
	while (!glfwWindowShouldClose(window)) {
		// accurate time measurement for synchronization
		currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - previousTime).count();
		previousTime = currentTime;
		timeSinceLaunch += deltaTime;

		/*counter++;
		accumulator += deltaTime;
		if (counter == 2000)
		{
			std::cout << (1 / accumulator) * 2000 << "\n";
			counter = 0;
			accumulator = 0;
		}*/
		//std::cout << deltaTime << "\n";

		glfwPollEvents();
		inputHandler.movePerson(deltaTime);
		restrictCharacterMovement(inputHandler.camera);

		drawFrame(timeSinceLaunch);
	}

	vkDeviceWaitIdle(vkInit.device);
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

void VulkanAndRTX::cleanupModels()
{
	for (size_t i = 0; i < models.objects.size(); i++) {
		vkDestroyBuffer(vkInit.device, models.objects[i].indexBuffer, nullptr);
		vkFreeMemory(vkInit.device, models.objects[i].indexBufferMemory, nullptr);

		vkDestroyBuffer(vkInit.device, models.objects[i].vertexBuffer, nullptr);
		vkFreeMemory(vkInit.device, models.objects[i].vertexBufferMemory, nullptr);
	}

	vkDestroyBuffer(vkInit.device, models.sky.indexBuffer, nullptr);
	vkFreeMemory(vkInit.device, models.sky.indexBufferMemory, nullptr);

	vkDestroyBuffer(vkInit.device, models.sky.vertexBuffer, nullptr);
	vkFreeMemory(vkInit.device, models.sky.vertexBufferMemory, nullptr);
}

// emptying RAM
void VulkanAndRTX::cleanupMemory()
{
	cleanupSwapChain();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer (vkInit.device, objectUniformBuffers[i], nullptr);
		vkFreeMemory    (vkInit.device, objectUniformBuffersMemory[i], nullptr);
		vkDestroyBuffer (vkInit.device, skyUniformBuffers[i], nullptr);
		vkFreeMemory    (vkInit.device, skyUniformBuffersMemory[i], nullptr);
	}

	// will free all allocated descriptor sets from this pool
	vkDestroyDescriptorPool(vkInit.device, descriptorPool, nullptr);

	vkDestroySampler(vkInit.device, textureSampler, nullptr);
	vkDestroyImageView(vkInit.device, textureImageView, nullptr);

	vkDestroyImage(vkInit.device, textureImage, nullptr);
	vkFreeMemory(vkInit.device, textureImageMemory, nullptr);

	vkDestroyDescriptorSetLayout(vkInit.device, descriptorSetLayout, nullptr);

	cleanupModels();

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

// reading bytecode files and returning its bytes
std::vector<char> VulkanAndRTX::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!" + filename);
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

void VulkanAndRTX::createDescriptorSets()
{
	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	
	// object descriptor sets
	for (size_t i = 0; i < descriptorSets.size(); i++) {
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;

		if (vkAllocateDescriptorSets(vkInit.device, &allocInfo, &descriptorSets[i].objects) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = objectUniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);
		
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImageView;
		imageInfo.sampler = textureSampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstSet = descriptorSets[i].objects;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstSet = descriptorSets[i].objects;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(vkInit.device, static_cast<uint32_t>(descriptorWrites.size()), 
			descriptorWrites.data(), 0, nullptr);
	}

	// sky descriptor sets
	for (size_t i = 0; i < descriptorSets.size(); i++) {
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;

		if (vkAllocateDescriptorSets(vkInit.device, &allocInfo, &descriptorSets[i].sky) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = skyUniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstSet = descriptorSets[i].sky;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(vkInit.device, static_cast<uint32_t>(descriptorWrites.size()),
			descriptorWrites.data(), 0, nullptr);
	}
}

void VulkanAndRTX::createDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout) const
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.pImmutableSamplers = nullptr; // Optional

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(vkInit.device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void VulkanAndRTX::createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 5 * static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 2 * static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(vkInit.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

// for specific queue family
void VulkanAndRTX::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = vkInit.findQueueFamilies(vkInit.physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

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

// wraping shader
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