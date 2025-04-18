#include "pch.h"
#include "AetherEngine.h"

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

void AetherEngine::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
	VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(vkInit.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vkInit.device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(vkInit.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	// connecting buffer memory and the buffer itself
	vkBindBufferMemory(vkInit.device, buffer, bufferMemory, 0);
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
	VkDeviceSize bufferSize = sizeof(Vertex) * mesh.vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, stagingBufferMemory
	);

	void* data;
	vkMapMemory(vkInit.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, mesh.vertices.data(), (size_t)bufferSize);

	// Validation Step: Check bone data
	Vertex* vertices = static_cast<Vertex*>(data);
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
		/*for (size_t j = 0; j < 4; j++) {
			std::cout << vertex.boneWeights[j] << " " << vertex.boneIDs[j] << "\n";
		}*/
	}

	vkUnmapMemory(vkInit.device, stagingBufferMemory);

	// giving perfomance boost by using device local memory
	createBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		mesh.vertexBuffer, mesh.vertexBufferMemory
	);

	copyBuffer(stagingBuffer, mesh.vertexBuffer, bufferSize);

	vkDestroyBuffer(vkInit.device, stagingBuffer, nullptr);
	vkFreeMemory(vkInit.device, stagingBufferMemory, nullptr);
}
void AetherEngine::createIndexBuffer(Mesh& mesh)
{
	VkDeviceSize bufferSize = sizeof(mesh.indices[0]) * mesh.indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, stagingBufferMemory
	);

	void* data;
	vkMapMemory(vkInit.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, mesh.indices.data(), (size_t)bufferSize);
	vkUnmapMemory(vkInit.device, stagingBufferMemory);

	createBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		mesh.indexBuffer, mesh.indexBufferMemory
	);

	copyBuffer(stagingBuffer, mesh.indexBuffer, bufferSize);

	vkDestroyBuffer(vkInit.device, stagingBuffer, nullptr);
	vkFreeMemory(vkInit.device, stagingBufferMemory, nullptr);
}
