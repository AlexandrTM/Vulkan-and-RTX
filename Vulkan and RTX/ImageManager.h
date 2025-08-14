#pragma once

#include "pch.h"
#include "VulkanInitializer.h"

namespace ImageManager
{
	VkImageView createImageView(
		VulkanInitializer& vkInit, VkImage image, VkFormat format,
		VkImageAspectFlags aspectFlags, uint32_t mipLevels
	);
}