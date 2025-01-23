#include "pch.h"
#include "VulkanAndRTX.h"

// creating swap chain with the best properties for current device
void VulkanAndRTX::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = vkInit.querySwapChainSupport(vkInit.physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	std::cout << "current present mode: " << presentMode << "\n";
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

	QueueFamilyIndices indices = vkInit.queueFamilyIndices;
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
	swapchainImageFormat = surfaceFormat.format;
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

void VulkanAndRTX::createSwapChainImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (uint32_t i = 0; i < swapChainImages.size(); i++) {
		swapChainImageViews[i] = createImageView(swapChainImages[i], swapchainImageFormat,
			VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

// creating framebuffer from each swap chain image view
void VulkanAndRTX::createSwapChainFramebuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		std::array<VkImageView, 3> attachments = {
			msaaTexture.imageView,
			depthTexture.imageView,
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = objectRenderPass;
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

// recreating swap chain in some special cases
void VulkanAndRTX::recreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	// stops rendering if window out of view
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
		ImGui_ImplVulkan_SetMinImageCount(MAX_FRAMES_IN_FLIGHT);
		ImGui_ImplVulkanH_CreateOrResizeWindow(
			vkInit.instance, vkInit.physicalDevice, vkInit.device, 
			vulkanWindow, vkInit.queueFamilyIndices.graphicsFamily.value(),
			nullptr, width, height, MAX_FRAMES_IN_FLIGHT
		);
		vulkanWindow->FrameIndex = 0;
	}

	// waiting for previous swap chain to stop rendering
	vkDeviceWaitIdle(vkInit.device);

	cleanupSwapChain();

	createPipelinesAndSwapchain();
	createColorTexture(msaaTexture);
	createDepthTexture(depthTexture);
	createSwapChainFramebuffers();
	createCommandBuffers();
	createDescriptorPool();
	createShaderBuffers(sky, MAX_FRAMES_IN_FLIGHT);
	createShaderBuffers(models, MAX_FRAMES_IN_FLIGHT);
	createDescriptorSets(sky, MAX_FRAMES_IN_FLIGHT);
	createDescriptorSets(models, MAX_FRAMES_IN_FLIGHT);
}

void VulkanAndRTX::cleanupSwapChain()
{
	cleanupTexture(depthTexture);
	cleanupTexture(msaaTexture);

	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(vkInit.device, swapChainFramebuffers[i], nullptr);
	}

	for (auto& pipeline : pipelines) {
		vkDestroyPipeline(vkInit.device, pipeline.second, nullptr);
	}
	vkDestroyPipelineLayout(vkInit.device, pipelineLayout, nullptr);

	vkDestroyRenderPass(vkInit.device, objectRenderPass, nullptr);

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(vkInit.device, swapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(vkInit.device, swapChain, nullptr);
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

// choosing best present mode to window surface
VkPresentModeKHR VulkanAndRTX::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		// unlimited framerate VK_PRESENT_MODE_IMMEDIATE_KHR
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

// choosing best swap chain extent(resolution of the images)
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
