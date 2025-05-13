#include "pch.h"
#include "AetherEngine.h"

void AetherEngine::initVMA() {
	VmaVulkanFunctions vulkanFunctions = {};
	vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
	vulkanFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
	vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
	vulkanFunctions.vkAllocateMemory = vkAllocateMemory;
	vulkanFunctions.vkFreeMemory = vkFreeMemory;
	vulkanFunctions.vkMapMemory = vkMapMemory;
	vulkanFunctions.vkUnmapMemory = vkUnmapMemory;
	vulkanFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
	vulkanFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
	vulkanFunctions.vkBindBufferMemory = vkBindBufferMemory;
	vulkanFunctions.vkBindImageMemory = vkBindImageMemory;
	vulkanFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
	vulkanFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
	vulkanFunctions.vkCreateBuffer = vkCreateBuffer;
	vulkanFunctions.vkDestroyBuffer = vkDestroyBuffer;
	vulkanFunctions.vkCreateImage = vkCreateImage;
	vulkanFunctions.vkDestroyImage = vkDestroyImage;
	vulkanFunctions.vkCmdCopyBuffer = vkCmdCopyBuffer;

	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.physicalDevice = vkInit.physicalDevice;
	allocatorInfo.device = vkInit.device;
	allocatorInfo.instance = vkInit.instance;
	allocatorInfo.pVulkanFunctions = &vulkanFunctions;

	if (vmaCreateAllocator(&allocatorInfo, &vmaAllocator) != VK_SUCCESS) {
		throw std::runtime_error("failed to create VMA allocator!");
	}
}

// allocating and beginning command buffer helper function
VkCommandBuffer AetherEngine::beginSingleTimeCommands() const
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
void AetherEngine::endSingleTimeCommands(VkCommandBuffer commandBuffer) const
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

//size_t totalSize = 0;
//size_t totalAllocationSize = 0;
std::map<size_t, size_t> bufferSizeCounts;
void AetherEngine::createBuffer(
	VkDeviceSize size, VkBufferUsageFlags usage, 
	VkMemoryPropertyFlags properties, bool isMappingRequired,
	VkBuffer& buffer, VmaAllocation& vmaAllocation
)
{
	bufferSizeCounts[size]++;
	//totalSize += size;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocCreateInfo.flags = properties;

	if (isMappingRequired) {
		allocCreateInfo.flags = 
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
			VMA_ALLOCATION_CREATE_MAPPED_BIT;
	}

	if (vmaCreateBuffer(vmaAllocator, &bufferInfo, &allocCreateInfo, &buffer, &vmaAllocation, nullptr) != VK_SUCCESS) {
		std::cout << "=== Unique Buffer Size Stats ===\n";
		for (const auto& [sz, count] : bufferSizeCounts) {
			std::cout << "Size: " << sz << " bytes - Count: " << count << "\n";
		}
		throw std::runtime_error("failed to allocate buffer memory with VMA!");
	}
	else {
		createdBuffers += 1;
	}
}
// create multiple command buffers
void AetherEngine::createCommandBuffers()
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

// copying contents of one buffer to another
void AetherEngine::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}
void AetherEngine::createVertexBuffer(Mesh& mesh)
{
	if (mesh.vertexBuffer != VK_NULL_HANDLE) {
		//std::cout << "mesh already have vertex buffer\n";
		return;
	}

	VkDeviceSize bufferSize = sizeof(Vertex) * mesh.vertices.size();

	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;

	createBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true,
		stagingBuffer, stagingAllocation
	);
	
	void* data;
	vmaMapMemory(vmaAllocator, stagingAllocation, &data);
	memcpy(data, mesh.vertices.data(), (size_t)bufferSize);

	// Validation Step: Check bone data
	/*Vertex* vertices = static_cast<Vertex*>(data);
	for (size_t i = 0; i < mesh.vertices.size(); ++i) {
		const Vertex& vertex = vertices[i];

		// Validate bone IDs
		for (uint32_t boneID : vertex.boneIDs) {
			if (boneID >= MAX_BONES_NUM) { // Assuming maxBones is defined elsewhere
				std::cerr << "Invalid bone ID " << boneID << " at vertex " << i << "\n";
			}
		}

		// Validate bone weights
		float weightSum = 0.0f;
		for (float weight : vertex.boneWeights) {
			if (weight < 0.0f) {
				std::cerr << "Negative bone weight " << weight << " at vertex " << i << "\n";
			}
			weightSum += weight;
		}

		if (std::abs(weightSum - 1.0f) > 0.01f) { // Allow small precision error
			//std::cerr << "Bone weights do not sum to 1.0 for vertex " << i << ". Sum: " << weightSum << "\n";
		}
		for (size_t j = 0; j < 4; j++) {
			std::cout << vertex.boneWeights[j] << " " << vertex.boneIDs[j] << "\n";
		}
	}*/

	vmaUnmapMemory(vmaAllocator, stagingAllocation);

	// giving perfomance boost by using device local memory
	createBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true,
		mesh.vertexBuffer, mesh.vertexBufferAllocation
	);

	copyBuffer(stagingBuffer, mesh.vertexBuffer, bufferSize);

	vmaDestroyBuffer(vmaAllocator, stagingBuffer, stagingAllocation);
	destroyedVmaAllocations += 1;
}
void AetherEngine::createIndexBuffer(Mesh& mesh)
{
	if (mesh.indexBuffer != VK_NULL_HANDLE) {
		//std::cout << "mesh already have index buffer\n";
		return;
	}

	VkDeviceSize bufferSize = sizeof(mesh.indices[0]) * mesh.indices.size();

	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;

	createBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true,
		stagingBuffer, stagingAllocation
	);

	void* data;
	vmaMapMemory(vmaAllocator, stagingAllocation, &data);
	memcpy(data, mesh.indices.data(), (size_t)bufferSize);
	vmaUnmapMemory(vmaAllocator, stagingAllocation);

	createBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true,
		mesh.indexBuffer, mesh.indexBufferAllocation
	);

	copyBuffer(stagingBuffer, mesh.indexBuffer, bufferSize);

	vmaDestroyBuffer(vmaAllocator, stagingBuffer, stagingAllocation);
	destroyedVmaAllocations += 1;
}

void AetherEngine::computeAABB_createVertexIndexBuffers(std::vector<Model>& models) 
{
	for (Model& model : models) {
		computeAABB_createVertexIndexBuffers(model);
	}
}
void AetherEngine::computeAABB_createVertexIndexBuffers(Model& model) 
{
	for (Mesh& mesh : model.meshes) {
		computeAABB_createVertexIndexBuffers(mesh);
	}
}
void AetherEngine::computeAABB_createVertexIndexBuffers(Mesh& mesh) 
{
	createVertexBuffer(mesh);
	createIndexBuffer(mesh);
	computeAABB(mesh);
}

void AetherEngine::createShaderBuffers(std::vector<Model>& models, size_t swapchainImageCount)
{
	for (Model& model : models) {
		createShaderBuffers(model, swapchainImageCount);
	}
}
void AetherEngine::createShaderBuffers(Model& model, size_t swapchainImageCount)
{
	for (Mesh& mesh : model.meshes) {
		createShaderBuffers(mesh, swapchainImageCount);
	}
}
void AetherEngine::createShaderBuffers(Mesh& mesh, size_t swapchainImageCount)
{
	for (size_t frameIndex = 0; frameIndex < swapchainImageCount; ++frameIndex) {
		if (mesh.UBOBuffers[frameIndex] == VK_NULL_HANDLE) {
			createBuffer(
				sizeof(UniformBufferObject),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true,
				mesh.UBOBuffers[frameIndex],
				mesh.UBOAllocations[frameIndex]
			);
		}

		if (mesh.bones.size() > 0 && mesh.boneSSBOBuffers[frameIndex] == VK_NULL_HANDLE) {
			createBuffer(
				sizeof(glm::mat4) * MAX_BONES_NUM,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true,
				mesh.boneSSBOBuffers[frameIndex],
				mesh.boneSSBOAllocations[frameIndex]
			);
		}
	}
}
