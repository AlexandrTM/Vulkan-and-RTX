#include "pch.h"
#include "AetherEngine.h"

// creating swap chain with the best properties for current device
void AetherEngine::createSwapchain()
{
	SwapchainSupportDetails swapchainSupport = vkInit.querySwapchainSupport(vkInit.physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapchainSurfaceFormat(swapchainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapchainPresentMode(swapchainSupport.presentModes);
	VkExtent2D extent = chooseSwapchainExtent(swapchainSupport.capabilities);

	/*std::cout << 
		"current present mode: " << presentMode << "\n" << 
		"extent: " << extent.width << " " << extent.height << "\n";*/

	uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

	if (swapchainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapchainSupport.capabilities.maxImageCount) {
		imageCount = swapchainSupport.capabilities.maxImageCount;
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

	createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.clipped = VK_TRUE;
	createInfo.presentMode = presentMode;
	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;

	if (vkCreateSwapchainKHR(vkInit.device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	// make sure that swap chain image count is adequate
	vkGetSwapchainImagesKHR(vkInit.device, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	// getting images from the swap chain
	vkGetSwapchainImagesKHR(vkInit.device, swapchain, &imageCount, swapchainImages.data());
}

void AetherEngine::createSwapchainImageViews()
{
	swapchainImageViews.resize(swapchainImages.size());

	for (uint32_t i = 0; i < swapchainImages.size(); i++) {
		swapchainImageViews[i] = createImageView(swapchainImages[i], swapchainImageFormat,
			VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

// creating framebuffer from each swap chain image view
void AetherEngine::createSwapchainFramebuffers()
{
	swapchainFramebuffers.resize(swapchainImageViews.size());

	for (size_t i = 0; i < swapchainImageViews.size(); i++) {
		std::array<VkImageView, 3> attachments = {
			msaaTexture.imageView,
			depthTexture.imageView,
			swapchainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = objectRenderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapchainExtent.width;
		framebufferInfo.height = swapchainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(vkInit.device, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer! " + i);
		}
	}
}

// recreating swap chain in some special cases
void AetherEngine::recreateSwapchain()
{
	vkDeviceWaitIdle(vkInit.device);

	/*if (vkResetDescriptorPool(vkInit.device, descriptorPool, 0) != VK_SUCCESS) {
		throw std::runtime_error("Failed to reset descriptor pool!");
	}*/

	cleanupSwapchain();
	cleanupShaderBuffers(sky);
	cleanupShaderBuffers(models);

	cleanupModel(pauseMenuModel);
	cleanupModel(inGameOverlayModel);
	cleanupModel(selectEquationModel);
	cleanupModel(solveEquationModel);
	/*cleanupTexture(pauseMenuTexture);
	cleanupTexture(inGameOverlayTexture);
	cleanupTexture(selectEquationTexture);
	cleanupTexture(solveEquationTexture);*/

	cleanupTexture(depthTexture);
	cleanupTexture(msaaTexture);

	createPipelinesAndSwapchain();
	createColorTexture(msaaTexture);
	createDepthTexture(depthTexture);
	createSwapchainFramebuffers();
	createCommandBuffers();
	
	// interface elements creation
	{
		createSolidColorTexture({ 0, 0, 0, 0 }, windowWidth, windowHeight, pauseMenuTexture);
		pauseMenuModel = ModelManager::createQuad(
			{ -1.0f, -1.0f, 0.0f }, { 2.0f, 2.0f },
			{ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f },
			glm::vec3(0.5f),
			pauseMenuTexture
		);
		computeAABB_createVertexIndexBuffers(pauseMenuModel);
		createDescriptorSets(pauseMenuModel, MAX_FRAMES_IN_FLIGHT);

		createSolidColorTexture({ 0, 0, 0, 0 }, windowWidth, windowHeight, inGameOverlayTexture);
		inGameOverlayModel = ModelManager::createQuad(
			{ -1.0f, -1.0f, 0.0f }, { 2.0f, 2.0f },
			{ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f },
			glm::vec3(0.5f),
			inGameOverlayTexture
		);
		computeAABB_createVertexIndexBuffers(inGameOverlayModel);
		createDescriptorSets(inGameOverlayModel, MAX_FRAMES_IN_FLIGHT);

		createSolidColorTexture({ 0, 0, 0, 0 }, windowWidth, windowHeight, selectEquationTexture);
		selectEquationModel = ModelManager::createQuad(
			{ -1.0f, -1.0f, 0.0f }, { 2.0f, 2.0f },
			{ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f },
			glm::vec3(0.5f),
			selectEquationTexture
		);
		computeAABB_createVertexIndexBuffers(selectEquationModel);
		createDescriptorSets(selectEquationModel, MAX_FRAMES_IN_FLIGHT);

		createSolidColorTexture({ 0, 0, 0, 0 }, windowWidth, windowHeight, solveEquationTexture);
		solveEquationModel = ModelManager::createQuad(
			{ -1.0f, -1.0f, 0.0f }, { 2.0f, 2.0f },
			{ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f },
			glm::vec3(0.5f),
			solveEquationTexture
		);
		computeAABB_createVertexIndexBuffers(solveEquationModel);
		createDescriptorSets(solveEquationModel, MAX_FRAMES_IN_FLIGHT);
	}

	createShaderBuffers(sky, MAX_FRAMES_IN_FLIGHT);
	createShaderBuffers(models, MAX_FRAMES_IN_FLIGHT);
	//createDescriptorSets(sky, MAX_FRAMES_IN_FLIGHT);
	//createDescriptorSets(models, MAX_FRAMES_IN_FLIGHT);
}

void AetherEngine::cleanupSwapchain()
{
	for (size_t i = 0; i < swapchainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(vkInit.device, swapchainFramebuffers[i], nullptr);
	}

	for (auto& pipeline : pipelines) {
		vkDestroyPipeline(vkInit.device, pipeline.second, nullptr);
	}
	vkDestroyPipelineLayout(vkInit.device, pipelineLayout, nullptr);

	vkDestroyRenderPass(vkInit.device, objectRenderPass, nullptr);
	vkDestroyRenderPass(vkInit.device, uiRenderPass, nullptr);

	for (size_t i = 0; i < swapchainImageViews.size(); i++) {
		vkDestroyImageView(vkInit.device, swapchainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(vkInit.device, swapchain, nullptr);
}

// choosing best surface format(color space and number of bits) for the swapchain
VkSurfaceFormatKHR AetherEngine::chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

// choosing best present mode to glfwWindow surface
VkPresentModeKHR AetherEngine::chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
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
VkExtent2D AetherEngine::chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		QSize windowSize = inGameWindow->size();

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(windowSize.width()),
			static_cast<uint32_t>(windowSize.height())
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height);

		return actualExtent;
	}
}
