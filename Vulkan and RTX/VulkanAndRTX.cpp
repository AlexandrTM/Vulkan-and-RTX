#include "pch.h"
#include "VulkanAndRTX.h"
#include "VulkanInitializer.h"
#include "InputHandler.h"
#include "Vertex.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


#pragma region

uint32_t windowWidth = 800;
uint32_t windowHeight = 450;
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::string MODEL_PATH = "models/model.obj";
const std::string TEXTURE_PATH = "textures/texture.png";

#pragma endregion

void VulkanAndRTX::run()
{
	createWindow();
	inputHandler.initializeInputHandler(window);
	vkInit.initializeVulkan(window);
	prepareResources();
	mainLoop();
	cleanup();
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

void VulkanAndRTX::prepareResources()
{
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout(descriptorSetLayout);
	createGraphicsPipeline();
	createCommandPool();
	createColorResources();
	createDepthResources();
	createFramebuffers();

	createTextureImage(TEXTURE_PATH, textureImage, textureImageMemory);
	createTextureImageView(textureImage, textureImageView);
	createTextureSampler(textureSampler);

	loadGltfModel("models/elipsoid low poly.gltf");
	//loadModel("models/elipsoid low poly.obj");
	generateCubicLandscape(15, 15, 1.0f);

	createVertexBuffer(vertices, vertexBuffer, vertexBufferMemory);
	createIndexBuffer(indices, indexBuffer, indexBufferMemory);
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

		drawFrame(timeSinceLaunch);
	}

	vkDeviceWaitIdle(vkInit.device);
}

// emptying RAM
void VulkanAndRTX::cleanup()
{
	cleanupSwapChain();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(vkInit.device, uniformBuffers[i], nullptr);
		vkFreeMemory(vkInit.device, uniformBuffersMemory[i], nullptr);
	}

	// will free all allocated descriptor sets from this pool
	vkDestroyDescriptorPool(vkInit.device, descriptorPool, nullptr);

	vkDestroySampler(vkInit.device, textureSampler, nullptr);
	vkDestroyImageView(vkInit.device, textureImageView, nullptr);

	vkDestroyImage(vkInit.device, textureImage, nullptr);
	vkFreeMemory(vkInit.device, textureImageMemory, nullptr);

	vkDestroyDescriptorSetLayout(vkInit.device, descriptorSetLayout, nullptr);

	vkDestroyBuffer(vkInit.device, indexBuffer, nullptr);
	vkFreeMemory(vkInit.device, indexBufferMemory, nullptr);

	vkDestroyBuffer(vkInit.device, vertexBuffer, nullptr);
	vkFreeMemory(vkInit.device, vertexBufferMemory, nullptr);

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

// cleaning "out of date" swap chain
void VulkanAndRTX::cleanupSwapChain()
{
	vkDestroyImageView(vkInit.device, depthImageView, nullptr);
	vkDestroyImage(vkInit.device, depthImage, nullptr);
	vkFreeMemory(vkInit.device, depthImageMemory, nullptr);

	vkDestroyImageView(vkInit.device, colorImageView, nullptr);
	vkDestroyImage(vkInit.device, colorImage, nullptr);
	vkFreeMemory(vkInit.device, colorImageMemory, nullptr);

	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(vkInit.device, swapChainFramebuffers[i], nullptr);
	}

	vkDestroyPipeline(vkInit.device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(vkInit.device, pipelineLayout, nullptr);
	vkDestroyRenderPass(vkInit.device, renderPass, nullptr);

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(vkInit.device, swapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(vkInit.device, swapChain, nullptr);
}

// recreating swap chain in some special cases
void VulkanAndRTX::recreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	// stops rendering if window out of view
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	// waiting for previous swap chain to stop rendering
	vkDeviceWaitIdle(vkInit.device);

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createColorResources();
	createDepthResources();
	createFramebuffers();
	createUniformBuffers();
	createCommandBuffers();
	createDescriptorPool();
	createDescriptorSets();
}

// set "framebufferResized" to "true" if window was resized or moved
void VulkanAndRTX::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<VulkanAndRTX*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
	inputHandler.camera.setViewportSize(width, height);
}

void VulkanAndRTX::generateCubicLandscape(size_t landscapeWidth, size_t landscapeLenght, float_t cubeSize)
{
	for (size_t i = 0; i < landscapeWidth; i++)
	{
		for (size_t j = 0; j < landscapeLenght; j++)
		{
			float random_height = 0.01 * (rand() % 51);
			generateCube(0.0f + (float)i * cubeSize - landscapeWidth / 4,
				-2 + random_height,
				0.0f + (float)j * cubeSize - landscapeLenght / 4,
				cubeSize);
		}
	}
}

void VulkanAndRTX::generateCube(float x, float y, float z, float_t cubeSize)
{
	glm::vec3 basicColor = glm::vec3(0.5f, 0.5f, 0.5f);

	std::vector <Vertex> localVertices(24);
#pragma region
	localVertices[0].pos = { glm::vec3(0.0f    , 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[1].pos = { glm::vec3(cubeSize, 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[2].pos = { glm::vec3(cubeSize, cubeSize, 0.0f) + glm::vec3(x, y, z) };
	localVertices[3].pos = { glm::vec3(0.0f    , cubeSize, 0.0f) + glm::vec3(x, y, z) };
	localVertices[4].pos = { glm::vec3(0.0f    , 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[5].pos = { glm::vec3(cubeSize, 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[6].pos = { glm::vec3(cubeSize, cubeSize, cubeSize) + glm::vec3(x, y, z) };
	localVertices[7].pos = { glm::vec3(0.0f    , cubeSize, cubeSize) + glm::vec3(x, y, z) };

	localVertices[8].pos = { glm::vec3(0.0f    , 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[9].pos = { glm::vec3(cubeSize, 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[10].pos = { glm::vec3(cubeSize, cubeSize, 0.0f) + glm::vec3(x, y, z) };
	localVertices[11].pos = { glm::vec3(0.0f    , cubeSize, 0.0f) + glm::vec3(x, y, z) };
	localVertices[12].pos = { glm::vec3(0.0f    , 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[13].pos = { glm::vec3(cubeSize, 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[14].pos = { glm::vec3(cubeSize, cubeSize, cubeSize) + glm::vec3(x, y, z) };
	localVertices[15].pos = { glm::vec3(0.0f    , cubeSize, cubeSize) + glm::vec3(x, y, z) };

	localVertices[16].pos = { glm::vec3(0.0f    , 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[17].pos = { glm::vec3(cubeSize, 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[18].pos = { glm::vec3(cubeSize, cubeSize, 0.0f) + glm::vec3(x, y, z) };
	localVertices[19].pos = { glm::vec3(0.0f    , cubeSize, 0.0f) + glm::vec3(x, y, z) };
	localVertices[20].pos = { glm::vec3(0.0f    , 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[21].pos = { glm::vec3(cubeSize, 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[22].pos = { glm::vec3(cubeSize, cubeSize, cubeSize) + glm::vec3(x, y, z) };
	localVertices[23].pos = { glm::vec3(0.0f    , cubeSize, cubeSize) + glm::vec3(x, y, z) };
#pragma endregion // pos
#pragma region
	localVertices[0].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[1].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[2].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[3].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[4].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };
	localVertices[5].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };
	localVertices[6].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };
	localVertices[7].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };

	localVertices[8].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[9].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[10].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };
	localVertices[11].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };
	localVertices[12].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[13].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[14].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };
	localVertices[15].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };

	localVertices[16].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
	localVertices[17].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[18].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[19].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
	localVertices[20].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
	localVertices[21].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[22].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[23].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
#pragma endregion // normal
#pragma region
	for (size_t i = 0; i < localVertices.size(); ++i)
	{
		localVertices[i].color = basicColor;
	}
#pragma endregion // color
#pragma region
	localVertices[0].texCoord = { glm::vec2(0.0f, 0.0f) };
	localVertices[1].texCoord = { glm::vec2(1.0f, 0.0f) };
	localVertices[2].texCoord = { glm::vec2(1.0f, 1.0f) };
	localVertices[3].texCoord = { glm::vec2(0.0f, 1.0f) };
	localVertices[4].texCoord = { glm::vec2(0.0f, 1.0f) };
	localVertices[5].texCoord = { glm::vec2(1.0f, 1.0f) };
	localVertices[6].texCoord = { glm::vec2(1.0f, 0.0f) };
	localVertices[7].texCoord = { glm::vec2(0.0f, 0.0f) };

	localVertices[8].texCoord = { glm::vec2(0.0f, 1.0f) };
	localVertices[9].texCoord = { glm::vec2(1.0f, 1.0f) };
	localVertices[10].texCoord = { glm::vec2(1.0f, 0.0f) };
	localVertices[11].texCoord = { glm::vec2(0.0f, 0.0f) };
	localVertices[12].texCoord = { glm::vec2(0.0f, 0.0f) };
	localVertices[13].texCoord = { glm::vec2(1.0f, 0.0f) };
	localVertices[14].texCoord = { glm::vec2(1.0f, 1.0f) };
	localVertices[15].texCoord = { glm::vec2(0.0f, 1.0f) };

	localVertices[16].texCoord = { glm::vec2(1.0f, 0.0f) };
	localVertices[17].texCoord = { glm::vec2(0.0f, 0.0f) };
	localVertices[18].texCoord = { glm::vec2(0.0f, 1.0f) };
	localVertices[19].texCoord = { glm::vec2(1.0f, 1.0f) };
	localVertices[20].texCoord = { glm::vec2(0.0f, 0.0f) };
	localVertices[21].texCoord = { glm::vec2(1.0f, 0.0f) };
	localVertices[22].texCoord = { glm::vec2(1.0f, 1.0f) };
	localVertices[23].texCoord = { glm::vec2(0.0f, 1.0f) };
#pragma endregion // texCoord

	uint32_t verticesNum = static_cast<uint32_t>(vertices.size());
	std::vector<uint32_t> localIndices = {
		0 + verticesNum, 3 + verticesNum, 1 + verticesNum,
		1 + verticesNum, 3 + verticesNum, 2 + verticesNum,
		4 + verticesNum, 5 + verticesNum, 7 + verticesNum,
		7 + verticesNum, 5 + verticesNum, 6 + verticesNum,

		11 + verticesNum, 15 + verticesNum, 10 + verticesNum,
		10 + verticesNum, 15 + verticesNum, 14 + verticesNum,
		12 + verticesNum, 8 + verticesNum, 13 + verticesNum,
		13 + verticesNum, 8 + verticesNum, 9 + verticesNum,

		20 + verticesNum, 23 + verticesNum, 16 + verticesNum,
		16 + verticesNum, 23 + verticesNum, 19 + verticesNum,
		17 + verticesNum, 18 + verticesNum, 21 + verticesNum,
		21 + verticesNum, 18 + verticesNum, 22 + verticesNum
	};

	// indices and vertices push back
	for (size_t i = 0; i < localVertices.size(); i++)
	{
		vertices.push_back(localVertices[i]);
	}
	for (size_t i = 0; i < localIndices.size(); i++)
	{
		indices.push_back(localIndices[i]);
	}
}

void VulkanAndRTX::loadModel(std::string modelPath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1 - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.f, 1.f, 1.f };

			vertex.normal = {
				attrib.normals[index.normal_index * 3],
				attrib.normals[index.normal_index * 3 + 1],
				attrib.normals[index.normal_index * 3 + 2]
			};

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}
}

void VulkanAndRTX::loadGltfModel(const std::string& filePath) {
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool result = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);

	if (!result) {
		std::cout << "model not loaded: " + filePath << "\n";
	}

	for (const auto& mesh : model.meshes) {
		for (const auto& primitive : mesh.primitives) {

			const float* bufferPos = nullptr;
			const float* bufferNormals = nullptr;

			const float* bufferTexCoordSet0 = nullptr;
			const float* bufferTexCoordSet1 = nullptr;
			const float* bufferColorSet0 = nullptr;
			const void* bufferJoints = nullptr;
			const float* bufferWeights = nullptr;

			const auto& attributes = primitive.attributes;

			// Extract POSITION
			if (attributes.find("POSITION") != attributes.end()) {
				const tinygltf::Accessor& positionAccessor = model.accessors[attributes.find("POSITION")->second];
				const tinygltf::BufferView& posView = model.bufferViews[positionAccessor.bufferView];
				bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[positionAccessor.byteOffset + posView.byteOffset]));
				
				if (attributes.find("NORMAL") != attributes.end()) {
					const tinygltf::Accessor& normalAccessor = model.accessors[attributes.find("NORMAL")->second];
					const tinygltf::BufferView& normalView = model.bufferViews[normalAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normalView.buffer].data[normalAccessor.byteOffset + normalView.byteOffset]));
				}

				// Extract INDICES
				const tinygltf::Accessor& indicesAccessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
				const tinygltf::BufferView& indicesView = model.bufferViews[indicesAccessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[indicesView.buffer];

				switch (indicesAccessor.componentType)
				{
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
					const uint8_t* indicesData = reinterpret_cast<const uint8_t*>
						(&(buffer.data[indicesAccessor.byteOffset + indicesView.byteOffset]));

					for (size_t i = 0; i < indicesAccessor.count; ++i) {
						indices.push_back(indicesData[i]);
					}
					break;
				}
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
					const uint16_t* indicesData = reinterpret_cast<const uint16_t*>
						(&(buffer.data[indicesAccessor.byteOffset + indicesView.byteOffset]));

					for (size_t i = 0; i < indicesAccessor.count; ++i) {
						indices.push_back(indicesData[i]);
					}
					break;
				}
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
					const uint32_t* indicesData = reinterpret_cast<const uint32_t*>
						(&(buffer.data[indicesAccessor.byteOffset + indicesView.byteOffset]));

					for (size_t i = 0; i < indicesAccessor.count; ++i) {
						indices.push_back(indicesData[i]);
					}
					break;
				}
				default:
					break;
				}		

				// Iterate through vertices
				//std::cout << "vertices: " << positionAccessor.count << "\n";
				for (size_t i = 0; i < positionAccessor.count; ++i) {
					Vertex vertex{};

					// Extract POSITION
					vertex.pos = glm::vec3(
						bufferPos[i * 3], 
						bufferPos[i * 3 + 1], 
						bufferPos[i * 3 + 2]);
					vertex.color = { 0.5f, 0.5f, 0.5f };
					//std::cout << vertex.pos[0] << vertex.pos[1] << vertex.pos[2] << "\n";

					// Extract NORMAL
					vertex.normal = glm::normalize(glm::vec3(bufferNormals ? glm::vec3(
						bufferNormals[i * 3    ], 
						bufferNormals[i * 3 + 1], 
						bufferNormals[i * 3 + 2]
					) : glm::vec3(0.0f)));
					//std::cout << vertex.normal[0] << vertex.normal[1] << vertex.normal[2] << "\n";
					
					// Extract TEXCOORD_0
					/*if (texCoordAccessor) {
						const float* texCoordData = reinterpret_cast<const float*>(&model.buffers[texCoordAccessor->indicesView].data[texCoordAccessor->byteOffset]);
						vertex.texCoord = glm::vec2(texCoordData[j * 2], texCoordData[j * 2 + 1]);
					}*/

					// Add the vertex to the vector
					vertices.push_back(vertex);
				}

				// Extract INDICES
				//if (indicesAccessor) {
				//	const uint32_t* indicesData = reinterpret_cast<const uint32_t*>(&model.buffers[indicesAccessor->indicesView].data[indicesAccessor->byteOffset]);
				//	//indices.insert(indices.end(), indicesData, indicesData + indicesAccessor->count);
				//}
			}
		}
	}
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

void VulkanAndRTX::createTextureImage(std::string texturePath, VkImage& textureImage, 
	VkDeviceMemory& textureImageMemory)
{
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;
	mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}

	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(vkInit.device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(vkInit.device, stagingBufferMemory);
	stbi_image_free(pixels);

	createImage(texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
		| VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		textureImage, textureImageMemory);

	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

	copyBufferToImage(stagingBuffer, textureImage, 
		static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	vkDestroyBuffer(vkInit.device, stagingBuffer, nullptr);
	vkFreeMemory(vkInit.device, stagingBufferMemory, nullptr);

	generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
}

void VulkanAndRTX::generateMipmaps(VkImage image, VkFormat imageFormat, 
	int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	// Check if image format supports linear blitting
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(vkInit.physicalDevice, imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		throw std::runtime_error("texture image format does not support linear blitting!");
	}

	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	for (uint32_t i = 1; i < mipLevels; i++) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(commandBuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	endSingleTimeCommands(commandBuffer);
}

// creating image for MSAA sampling
void VulkanAndRTX::createColorResources()
{
	VkFormat colorFormat = swapChainImageFormat;

	createImage(swapChainExtent.width, swapChainExtent.height, 1, vkInit.msaaSamples, colorFormat,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory);

	colorImageView = createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

// three objects for depth testing
void VulkanAndRTX::createDepthResources()
{
	VkFormat depthFormat = findDepthFormat();

	createImage(swapChainExtent.width, swapChainExtent.height, 1, vkInit.msaaSamples, depthFormat,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);

	depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

// check if the format has stencil component
bool VulkanAndRTX::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

// finding most appropriate format for the depth test
VkFormat VulkanAndRTX::findDepthFormat()
{
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

// finding most desirable format of color for a given situation
VkFormat VulkanAndRTX::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
	VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(vkInit.physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

// how to sample through texels of the texture for drawing them on 3D model
void VulkanAndRTX::createTextureSampler(VkSampler& textureSampler)
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(vkInit.physicalDevice, &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	samplerInfo.anisotropyEnable = VK_TRUE;

	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = static_cast<float>(mipLevels);
	samplerInfo.mipLodBias = 0.0f;

	if (vkCreateSampler(vkInit.device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void VulkanAndRTX::createTextureImageView(VkImage& textureImage, VkImageView& textureImageView)
{
	textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT,
		mipLevels);
}

VkImageView VulkanAndRTX::createImageView(VkImage image, VkFormat format, 
	VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(vkInit.device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

// transitioning image to the right layout
void VulkanAndRTX::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
	VkImageLayout newLayout, uint32_t mipLevels)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	// used for transitioning image from one layout to another
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	// used for transitioning ownership of the queue if VK_SHARING_MODE_EXCLUSIVE is used
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}


	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	endSingleTimeCommands(commandBuffer);
}

void VulkanAndRTX::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	endSingleTimeCommands(commandBuffer);
}

// allocating and beginning command buffer helper function
VkCommandBuffer VulkanAndRTX::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(vkInit.device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

// ending and submitting command buffer helper function
void VulkanAndRTX::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(vkInit.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(vkInit.graphicsQueue);

	vkFreeCommandBuffers(vkInit.device, commandPool, 1, &commandBuffer);
}

void VulkanAndRTX::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
	VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
	VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = numSamples;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(vkInit.device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(vkInit.device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(vkInit.device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(vkInit.device, image, imageMemory, 0);
}

// finding most appropriate memory type depending on buffer and application properties
uint32_t VulkanAndRTX::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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

// copying contents of one buffer to another
void VulkanAndRTX::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void VulkanAndRTX::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
	VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	// which queue will use the buffer
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(vkInit.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vkInit.device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(vkInit.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	// connecting buffer memory and the buffer itself
	vkBindBufferMemory(vkInit.device, buffer, bufferMemory, 0);
}

void VulkanAndRTX::createVertexBuffer(const std::vector<Vertex>& vertices,
	VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory)
{
	VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(vkInit.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(vkInit.device, stagingBufferMemory);
		
	// giving perfomance boost by using device local memory
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(vkInit.device, stagingBuffer, nullptr);
	vkFreeMemory(vkInit.device, stagingBufferMemory, nullptr);
}

void VulkanAndRTX::createIndexBuffer(const std::vector<uint32_t>& indices,
	VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory)
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(vkInit.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(vkInit.device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(vkInit.device, stagingBuffer, nullptr);
	vkFreeMemory(vkInit.device, stagingBufferMemory, nullptr);
}

void VulkanAndRTX::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(vkInit.device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);
		
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImageView;
		imageInfo.sampler = textureSampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(vkInit.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),
			0, nullptr);
	}
}

// create multiple command buffers
void VulkanAndRTX::createCommandBuffers()
{
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(vkInit.device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

// record commands to the command buffer
void VulkanAndRTX::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
		&descriptorSets[currentFrame], 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

// creating uniform buffer for each frame in flight
void VulkanAndRTX::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
	}
}

void VulkanAndRTX::createDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
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
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(vkInit.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

// creating swap chain with the best properties for current device
void VulkanAndRTX::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = vkInit.querySwapChainSupport(vkInit.physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	std::cout << "current present mode:" << presentMode << "\n";
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = vkInit.surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = vkInit.findQueueFamilies(vkInit.physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.clipped = VK_TRUE;
	createInfo.presentMode = presentMode;
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	if (vkCreateSwapchainKHR(vkInit.device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	// make sure that swap chain image count is adequate
	vkGetSwapchainImagesKHR(vkInit.device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	// getting images from the swap chain
	vkGetSwapchainImagesKHR(vkInit.device, swapChain, &imageCount, swapChainImages.data());
}

void VulkanAndRTX::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (uint32_t i = 0; i < swapChainImages.size(); i++) {
		swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat,
			VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

// information about framebuffer attachments, how many color and depth buffers there will
// be, how many samples to use for each of them and how their contents should be treated
void VulkanAndRTX::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = vkInit.msaaSamples;// samples per fragment		
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;// color and depth data
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;// stencil data
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// in which layout image will be before render pass
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// in which layout image will be transitioned after render pass
	// images to be presented in the swap chain
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = vkInit.msaaSamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = swapChainImageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;// index of attachment description
	// which layout we would like the attachment to have during a subpass that uses this reference
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = 2;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(vkInit.device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

// transfering scene to images
void VulkanAndRTX::createGraphicsPipeline()
{
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");


	/* printing bits */
	/*std::cout << vertShaderCode.size();
	std::cout << fragShaderCode.size();

	int j = 0;
	for (auto i : vertShaderCode) {
		j++;
		std::bitset<8> a(i);
		std::cout << a << " ";
		if (j == 12) { j = 0; std::cout << "\n"; }
	}*/

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	// Vertex shader stage   (NOT FIXED): moving vertices in relation to the camera
#pragma region
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
#pragma endregion

	// Fragment shader stage (NOT FIXED): coloring fragments returned by rasterization stage
#pragma region
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";
#pragma endregion

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	// Vertex input stage	 (FIXED)
#pragma region
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional
#pragma endregion

	// Input Assembler stage (FIXED)
#pragma region
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
#pragma endregion

	// Viewport stage		 (FIXED): scissor and viewport
#pragma region	
// the region of the framebuffer that the output will be rendered to
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// filtering the pixel that was taken from the rasterizer, in our case its all pixels
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent.height = swapChainExtent.height;
	scissor.extent.width = swapChainExtent.width;

	// combining previous two features, its can be more that one scissor rectangle and viewport
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
#pragma endregion

	// Rasterization stage   (FIXED): converting geometry to fragments
#pragma region
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // (FILL, LINE, POINT)
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
#pragma endregion

	// Multisampling stage   (FIXED): how many samples for each fragment we will take also depth testing
#pragma region
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = vkInit.msaaSamples;
	multisampling.minSampleShading = 1.0f; // Optional

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

	depthStencil.depthBoundsTestEnable = VK_FALSE; // used for the specific depth test range
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional

	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional
#pragma endregion

	// Color blending stage  (FIXED): blending color returned by fragment shader with previous color
#pragma region
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	/* alternative case */
	/*
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	*/

	/* general case pseudo code */
	/*
	if (blendEnable) {
		finalColor.rgb = (srcColorBlendFactor * newColor.rgb) < colorBlendOp > (dstColorBlendFactor * oldColor.rgb);
		finalColor.a = (srcAlphaBlendFactor * newColor.a) < alphaBlendOp > (dstAlphaBlendFactor * oldColor.a);
	}
	else {
		finalColor = newColor;
	}

	finalColor = finalColor & colorWriteMask;
	*/

	/* alternative case pseudo code */
	/*
	finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
	finalColor.a = newAlpha.a;
	*/

	// constants for afformentioned for color blending calculations
	// this method has higher priority then previous
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional
#pragma endregion

	// altering some variables without pipeline recreation
#pragma region
// altering some variables in graphics pipeline that can be changed without pipeline recreation
	std::vector<VkDynamicState> dynamicStates = {
		//VK_DYNAMIC_STATE_VIEWPORT,
		//VK_DYNAMIC_STATE_SCISSOR
		//VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	// altering variables for shaders without their recreation
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(vkInit.device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
#pragma endregion

	// information about all stages
	VkGraphicsPipelineCreateInfo pipelineInfo{};
#pragma region
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	// NOT FIXED functions
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	// FIXED functions
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
#pragma endregion

	if (vkCreateGraphicsPipelines(vkInit.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(vkInit.device, fragShaderModule, nullptr);
	vkDestroyShaderModule(vkInit.device, vertShaderModule, nullptr);
}

// creating framebuffer from each swap chain image view
void VulkanAndRTX::createFramebuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		std::array<VkImageView, 3> attachments = {
			colorImageView,
			depthImageView,
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(vkInit.device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer! " + i);
		}
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

// Creating frames for presentation
void VulkanAndRTX::drawFrame(float timeSinceLaunch)
{
	vkWaitForFences(vkInit.device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(vkInit.device, swapChain, UINT64_MAX,
		imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// updating MVP matrix
	//updateUniformBuffer(imageIndex);
	updateUniformBuffer(currentFrame, timeSinceLaunch);


	// Only reset the fence if we are submitting work
	vkResetFences(vkInit.device, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);


	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(vkInit.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(vkInit.presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// updating MVP matrix for every draw call every frame
void VulkanAndRTX::updateUniformBuffer(uint32_t currentImage, float timeSinceLaunch)
{
	UniformBufferObject ubo{};
	ubo.model = glm::mat4(1.0f);

	ubo.view = glm::lookAt(inputHandler.camera.getLookFrom(), inputHandler.camera.getLookFrom() + inputHandler.camera.getDirection(),
		inputHandler.camera.getVerticalWorldAxis());

	ubo.projection = glm::perspective(glm::radians(inputHandler.camera.getVerticalFov()),
		swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 40.0f);
	ubo.projection[1][1] *= -1;

	ubo.sun = glm::vec3(std::cos(timeSinceLaunch * glm::two_pi<double>() / 10) * 3, 
						3.f, 
						std::sin(timeSinceLaunch * glm::two_pi<double>() / 10) * 3);
	ubo.viewer = inputHandler.camera.getLookFrom();

	// add transpose(inverse(ubo.model)) if doing non uniform scaling

	void* data;
	vkMapMemory(vkInit.device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(vkInit.device, uniformBuffersMemory[currentImage]);
}

// wraping shader
VkShaderModule VulkanAndRTX::createShaderModule(const std::vector<char>& code)
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

// choosing best surface format(color space and number of bits) for the swap chain
VkSurfaceFormatKHR VulkanAndRTX::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

// chosing best present mode to window surface
VkPresentModeKHR VulkanAndRTX::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		// unlimited framerate
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else {
			// triple buffering
			return VK_PRESENT_MODE_FIFO_KHR;
		}
	}
	return VK_PRESENT_MODE_IMMEDIATE_KHR;
}

// chosing best swap chain extent(resolution of the images)
VkExtent2D VulkanAndRTX::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

int runVulkanAndRTX() {
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