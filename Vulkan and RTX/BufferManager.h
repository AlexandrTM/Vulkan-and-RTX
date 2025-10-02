#pragma once

#include "pch.h"
#include "Model.h"
#include "VulkanInitializer.h"
#include "gamecontext_instance.h"

class BufferManager
{
public:
    BufferManager(VulkanInitializer& vkInit);

    void prepareModel(Model& model, size_t swapchainImageCount);
    void prepareModels(std::vector<Model>& models, size_t swapchainImageCount);

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

    void createShaderBuffers(std::vector<Model>& models, size_t swapchainImageCount);
    void createShaderBuffers(Model& model, size_t swapchainImageCount);
    void createShaderBuffers(Mesh& mesh, size_t swapchainImageCount);

private:
    VulkanInitializer& vkInit;

    // tracking stats
    std::map<size_t, size_t> bufferSizeCounts;

    void createVertexBuffer(Mesh& mesh);
    void createIndexBuffer(Mesh& mesh);

    void createDescriptorSets(std::vector<Model>& models, size_t swapchainImageCount);
    void createDescriptorSets(Model& model, size_t swapchainImageCount);
    void createDescriptorSets(Mesh& mesh, size_t swapchainImageCount);
    void createDescriptorSet(VkDescriptorSet& descriptorSet);
};