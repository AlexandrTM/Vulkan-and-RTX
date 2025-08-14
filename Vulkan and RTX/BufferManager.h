#pragma once

#include "pch.h"
#include "VulkanInitializer.h"
#include "Model.h"

class BufferManager
{
public:
    BufferManager(VulkanInitializer& vkInit);

    // allocating and beginning command buffer helper function
    VkCommandBuffer beginSingleTimeCommands() const;
    // ending and submitting command buffer helper function
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

    void createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        bool isMappingRequired,
        VkBuffer& buffer,
        VmaAllocation& allocation
    );
    void createCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers);

    // copying contents of one buffer to another
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;

    void createVertexBuffer(Mesh& mesh);
    void createIndexBuffer(Mesh& mesh);

    void computeAABB_createVertexIndexBuffers(std::vector<Model>& models);
    void computeAABB_createVertexIndexBuffers(Model& model);
    void computeAABB_createVertexIndexBuffers(Mesh& mesh);

    void createShaderBuffers(std::vector<Model>& models, size_t swapchainImageCount);
    void createShaderBuffers(Model& model, size_t swapchainImageCount);
    void createShaderBuffers(Mesh& mesh, size_t swapchainImageCount);

private:
    VulkanInitializer& vkInit;

    // tracking stats
    std::map<size_t, size_t> bufferSizeCounts;
};