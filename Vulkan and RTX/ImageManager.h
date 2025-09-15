#pragma once

#include "pch.h"
#include "VulkanInitializer.h"
#include "BufferManager.h"

class ImageManager
{
public:
	ImageManager(VulkanInitializer& vkInit, BufferManager& bufferManager);

	VkFormat findDepthFormat() const;
	// finding most desirable format of color for a given situation
	VkFormat findSupportedFormat(
		const std::vector<VkFormat>& candidates,
		VkImageTiling tiling, VkFormatFeatureFlags features
	) const;
	// check if the format has stencil component
	bool hasStencilComponent(VkFormat format);

	// how to sample through texels of the texture for drawing them on 3D model
	void createTextureSampler(uint32_t& mipLevels, VkSampler& textureSampler) const;

	VkImageView createImageView(
		VulkanInitializer& vkInit, VkImage image, VkFormat format,
		VkImageAspectFlags aspectFlags, uint32_t mipLevels
	);
	void generateMipmaps(
		VkImage image, VkFormat imageFormat,
		int32_t texWidth, int32_t texHeight, uint32_t mipLevels
	);
	void createImage(
		uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VmaAllocation& vmaAllocation
	);

	void copyBufferToImage(
		VkBuffer buffer, VkImage image, uint32_t width, uint32_t height
	);
	void transitionImageLayout(
		VkImage image, VkFormat format, VkImageAspectFlags aspectMask,
		VkImageLayout oldLayout, VkImageLayout newLayout,
		uint32_t mipLevels
	);

private:
	VulkanInitializer& vkInit;
	BufferManager& bufferManager;

};
