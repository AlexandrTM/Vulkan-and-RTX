#include "pch.h"
#include "AetherEngine.h"
#include "StellarCalculations.h"

// information about framebuffer attachments, how many color and depth buffers there will
// be, how many samples to use for each of them and how their contents should be treated
void AetherEngine::createObjectRenderPass(VkRenderPass& renderPass) const
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = vkInit.colorSamples;// samples per fragment		
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
	depthAttachment.samples = vkInit.colorSamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = swapchainImageFormat;
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
	dependency.srcStageMask = 
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | 
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = 
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | 
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = 
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | 
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

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
void AetherEngine::createUiRenderPass(VkRenderPass& renderPass) const
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;  // Preserve content
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(vkInit.device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void AetherEngine::createPipelineLayout(
	VkDescriptorSetLayout& descriptorSetLayout, 
	VkPipelineLayout& pipelineLayout
) const
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(vkInit.device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void AetherEngine::createGraphicsPipeline(
	const PipelineType pipelineType, const std::string pipelineName, 
	const std::string& vertexShader, const std::string& fragmentShader,
	const VkRenderPass& renderPass 
)
{
	auto vertShader = readFile(vertexShader);
	auto fragShader = readFile(fragmentShader);

	VkShaderModule vertShaderModule = createShaderModule(vertShader);
	VkShaderModule fragShaderModule = createShaderModule(fragShader);
	
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
	auto attributeDescriptions = Vertex::getAttributeDescriptions(
		pipelineType == 
			PipelineType::OBJECT ? VertexLayoutType::WHOLE : 
			pipelineType == PipelineType::GUI ? VertexLayoutType::POSITION_TEXCOORDS : 
			VertexLayoutType::POSITION_ONLY
	);

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
	viewport.height = (float)swapchainExtent.height;
	viewport.width = (float)swapchainExtent.width;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// filtering the pixel that was taken from the rasterizer, in our case its all pixels
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent.height = swapchainExtent.height;
	scissor.extent.width = swapchainExtent.width;

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

	// Multi sampling stage  (FIXED): how many samples for each fragment we will take also depth testing
#pragma region
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = 
		/*pipelineType == PipelineType::GUI ? VK_SAMPLE_COUNT_1_BIT : */
		vkInit.colorSamples;
	multisampling.minSampleShading = 1.0f; // Optional

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = pipelineType == PipelineType::OBJECT ? VK_TRUE : VK_FALSE;
	depthStencil.depthWriteEnable = pipelineType == PipelineType::OBJECT ? VK_TRUE : VK_FALSE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.front = depthStencil.back;
	depthStencil.back.compareOp = VK_COMPARE_OP_ALWAYS;

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
	colorBlendAttachment.colorWriteMask = 
		  VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	if (pipelineType == PipelineType::GUI) {
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		/*colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;*/
	}
	else {
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	}
	
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

	// constants for aforementioned color blending calculations
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

	// altering variables for shaders without their recreation
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();
#pragma endregion
	
	createPipelineLayout(descriptorSetLayout, pipelineLayout);

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
	
	VkPipeline pipeline{};
	if (vkCreateGraphicsPipelines(vkInit.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
		std::cout << pipelineName << " ";
		throw std::runtime_error("failed to create graphics pipeline!");
	}
	pipelines[pipelineName] = pipeline;

	vkDestroyShaderModule(vkInit.device, fragShaderModule, nullptr);
	vkDestroyShaderModule(vkInit.device, vertShaderModule, nullptr);
}

void AetherEngine::createPipelinesAndSwapchain()
{
	createSwapchain();
	createSwapchainImageViews();
	createObjectRenderPass(objectRenderPass);
	createUiRenderPass(uiRenderPass);
	createGraphicsPipeline(
		PipelineType::OBJECT,
		"object", "shaders/object.vert.spv", "shaders/object.frag.spv",
		objectRenderPass
	);
	createGraphicsPipeline(
		PipelineType::SKY,
		"sky", "shaders/sky.vert.spv", "shaders/sky.frag.spv",
		objectRenderPass
	);
	createGraphicsPipeline(
		PipelineType::GUI,
		"ui", "shaders/ui.vert.spv", "shaders/ui.frag.spv",
		objectRenderPass
	);
}

// Creating frames for presentation
void AetherEngine::drawFrame(double timeSinceLaunch, double deltaTime)
{
	vkWaitForFences(vkInit.device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(
		vkInit.device, swapchain, UINT64_MAX,
		imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex
	);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	updateShaderBuffers(currentFrame, timeSinceLaunch);

	// Only reset the fence if we are submitting work
	vkResetFences(vkInit.device, 1, &inFlightFences[currentFrame]);
	vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);

	recordCommandBuffer(
		commandBuffers[currentFrame], imageIndex, 
		timeSinceLaunch, deltaTime
	);

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];	
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(vkInit.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkSwapchainKHR swapchains[] = { swapchain };

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(vkInit.presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || isFramebufferResized) {
		isFramebufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void AetherEngine::updateShaderBuffers(uint32_t currentImage, double timeSinceLaunch)
{
	glm::mat4 view = glm::lookAt(
		character.camera.getPosition(),
		character.camera.getPosition() + character.camera.getDirection(),
		character.camera.getVerticalWorldAxis()
	);

	//std::cout << "aspect ratio: " << swapchainExtent.width / (float)swapchainExtent.height << "\n";
	//std::cout << "swapchain size: " << swapchainExtent.width << " " << swapchainExtent.height << "\n";
	
	glm::mat4 projection = glm::perspective(
		glm::radians(character.camera.getVerticalFov()),
		swapchainExtent.width / (float)swapchainExtent.height, 
		0.01f, 100000.0f
	);
	projection[1][1] *= -1;

	glm::vec3 sun = StellarCalculations::calculateSunPosition(timeSinceLaunch);
	//glm::vec3 sun = StellarCalculations::calculateSPASunPosition();

	// add transpose(inverse(ubo.model)) if doing non uniform scaling

	skyUBO.model = glm::mat4(glm::mat3(view));
	skyUBO.view = view;
	skyUBO.projection = projection;
	skyUBO.sun = sun;
	skyUBO.observer = character.camera.getPosition();

	void* data;
	for (Mesh& mesh : sky.meshes) {
		vmaMapMemory(vmaAllocator, mesh.UBOAllocations[currentImage], &data);
		memcpy(data, &skyUBO, sizeof(UniformBufferObject));
		vmaUnmapMemory(vmaAllocator, mesh.UBOAllocations[currentImage]);
	}

	// Update per-mesh shader buffers
	for (const Model& model : models) {
		for (const Mesh& mesh : model.meshes) {
			// update bones
			if (mesh.bones.size() > 0) {
				//std::cout << "mesh.bones.size(): " << mesh.bones.size() << "\n";
				for (size_t i = 0; i < mesh.bones.size(); i++) {
					boneSSBO.boneTransforms[i] = mesh.bones[i].finalTransform;
					//std::cout << glm::to_string(boneUBO.boneTransforms[i]) << "\n";
				}

				vmaMapMemory(vmaAllocator, mesh.boneSSBOAllocations[currentImage], &data);
				memcpy(data, boneSSBO.boneTransforms.data(), sizeof(glm::mat4)* MAX_BONES_NUM);
				vmaUnmapMemory(vmaAllocator, mesh.boneSSBOAllocations[currentImage]);

				/*glm::mat4* mappedMatrices = reinterpret_cast<glm::mat4*>(data);
				for (size_t i = 0; i < MAX_BONES_NUM; ++i) {
					std::cout << "Matrix " << i << ": " << glm::to_string(mappedMatrices[i]) << std::endl;
				}*/
			}

			//meshUBO.model = mesh.transform;
			meshUBO.model = glm::mat4(1.0);
			meshUBO.view = view;
			meshUBO.projection = projection;
			meshUBO.sun = sun;
			meshUBO.observer = character.camera.getPosition();

			vmaMapMemory(vmaAllocator, mesh.UBOAllocations[currentImage], &data);
			memcpy(data, &meshUBO, sizeof(UniformBufferObject));
			vmaUnmapMemory(vmaAllocator, mesh.UBOAllocations[currentImage]);
		}
	}
}
void AetherEngine::recordCommandBuffer(
	VkCommandBuffer commandBuffer, uint32_t imageIndex, 
	double timeSinceLaunch, double deltaTime
)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo objectRenderPassInfo{};
	objectRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	objectRenderPassInfo.renderPass = objectRenderPass;
	objectRenderPassInfo.framebuffer = swapchainFramebuffers[imageIndex];
	objectRenderPassInfo.renderArea.offset = { 0, 0 };
	objectRenderPassInfo.renderArea.extent = swapchainExtent;
	objectRenderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	objectRenderPassInfo.pClearValues = clearValues.data();

	VkRenderPassBeginInfo uiRenderPassInfo{};
	uiRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	uiRenderPassInfo.renderPass = uiRenderPass;
	uiRenderPassInfo.framebuffer = swapchainFramebuffers[imageIndex];
	uiRenderPassInfo.renderArea.offset = { 0, 0 };
	uiRenderPassInfo.renderArea.extent = swapchainExtent;
	uiRenderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	uiRenderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &objectRenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	if (!isFramebufferResized) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines["sky"]);
		recordModelToCommandBuffer(sky, commandBuffer);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines["object"]);
		recordModelsToCommandBuffer(models, commandBuffer);

		if (gameContext.currentGameState == GameState::MAIN_MENU) {
			renderQmlToTexture(mainMenu.renderer.get(), mainMenu.texture);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines["ui"]);
			recordModelToCommandBuffer(mainMenu.model, commandBuffer);
		}
		if (gameContext.currentGameState == GameState::SETTINGS_MENU) {
			renderQmlToTexture(settingsMenu.renderer.get(), settingsMenu.texture);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines["ui"]);
			recordModelToCommandBuffer(settingsMenu.model, commandBuffer);
		}
		if (gameContext.currentGameState == GameState::PAUSED) {
			renderQmlToTexture(pauseMenu.renderer.get(), pauseMenu.texture);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines["ui"]);
			recordModelToCommandBuffer(pauseMenu.model, commandBuffer);
		}
		if (gameContext.currentGameState == GameState::DUNGEON_EXPLORATION			 ||
			gameContext.currentGameState == GameState::IN_GAME_TESTING				 ||
			gameContext.currentGameState == GameState::COMBAT_PLAYER_SELECT_EQUATION ||
			gameContext.currentGameState == GameState::COMBAT_PLAYER_SOLVE_EQUATION  ||
			gameContext.currentGameState == GameState::COMBAT_MOB_TURN				 ||
			gameContext.currentGameState == GameState::DUNGEON_ROOM_CLEANED          ||
			gameContext.currentGameState == GameState::PAUSED) {

			renderQmlToTexture(inGameOverlay.renderer.get(), inGameOverlay.texture);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines["ui"]);
			recordModelToCommandBuffer(inGameOverlay.model, commandBuffer);
		}
		if (gameContext.currentGameState == GameState::COMBAT_PLAYER_SELECT_EQUATION) {

			renderQmlToTexture(selectEquation.renderer.get(), selectEquation.texture);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines["ui"]);
			recordModelToCommandBuffer(selectEquation.model, commandBuffer);
		}
		if (gameContext.currentGameState == GameState::COMBAT_PLAYER_SOLVE_EQUATION) {

			renderQmlToTexture(solveEquation.renderer.get(), solveEquation.texture);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines["ui"]);
			recordModelToCommandBuffer(solveEquation.model, commandBuffer);
		}
	}

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void AetherEngine::updateSolveEquation() {
	if (!isSolveEquationTextFieldActivated) {
		solveEquation.renderer->getQuickWindow()->requestActivate();
		isSolveEquationTextFieldActivated = true;
	}
	auto contentItem = solveEquation.renderer->getQuickWindow()->contentItem();
	if (!contentItem->hasFocus()) {
		contentItem->setFocus(true);
	}

	if (!solveEquation.renderer) return;
	auto rootItem = solveEquation.renderer->getRootItem();
	if (!rootItem) return;

	QObject* answerInput = rootItem->findChild<QObject*>("answerInput");
	if (answerInput) { answerInput->setProperty("focus", true); }

	QObject* expression = rootItem->findChild<QObject*>("expression");
	QObject* timeRemaining = rootItem->findChild<QObject*>("timeRemaining");

	if (expression) {
		expression->setProperty(
			"value", 
			QString::fromStdString(gameContext.selectedEquation->expression)
		);
	}
	if (timeRemaining) {
		double remainingTime = gameContext.timeRemainingToSolveEquation;
		double timeToSolve = gameContext.selectedEquation->timeToSolve;
		
		QString timeText = QString("%1s").arg(remainingTime, 0, 'f', 2);
		timeRemaining->setProperty("value", timeText);

		double t = std::clamp(1.0 - (remainingTime / timeToSolve), 0.0, 1.0);
		// Start fading only after fraction of time has passed
		double timeChangeOffset = 0.0;
		t = (t - timeChangeOffset) / (1.0 - timeChangeOffset); 
		t = std::clamp(t, 0.0, 1.0);

		int r = 255;
		int g = static_cast<int>(255 * (1.0 - t) + 100 * t);
		int b = static_cast<int>(255 * (1.0 - t) + 100 * t);

		QColor color = interpolateColor(QColor(255, 255, 255, 255), QColor(210, 100, 100, 255), t);
		timeRemaining->setProperty("dynamicColor", color);
	}
}
QColor AetherEngine::interpolateColor(const QColor& from, const QColor& to, double t) {
	t = std::clamp(t, 0.0, 1.0);
	int r = static_cast<int>(from.red()   * (1.0 - t) + to.red()   * t);
	int g = static_cast<int>(from.green() * (1.0 - t) + to.green() * t);
	int b = static_cast<int>(from.blue()  * (1.0 - t) + to.blue()  * t);
	int a = static_cast<int>(from.alpha() * (1.0 - t) + to.alpha() * t);
	return QColor(r, g, b, a);
}
void AetherEngine::clearSolveEquationInput() {
	auto rootItem = solveEquation.renderer->getRootItem();
	QObject* answerInput = rootItem->findChild<QObject*>("answerInput");
	if (answerInput) { answerInput->setProperty("text", ""); }
	solveEquation.renderer->getQuickWindow()->contentItem()->setFocus(false);
}
void AetherEngine::updateSelectEquation(size_t amountOfEquations) {
	gameContext.equations = Equations::generateEquations(amountOfEquations, 1);

	const auto& equations = gameContext.equations;

	if (!selectEquation.renderer) return;
	auto rootItem = selectEquation.renderer->getRootItem();
	if (!rootItem) return;

	for (size_t i = 0; i < amountOfEquations; ++i) {
		QObject* difficultyItem = rootItem->findChild<QObject*>(QString("difficulty%1").arg(i));
		if (!difficultyItem) continue;

		difficultyItem->setProperty("value", gameContext.equations[i].difficulty);		
	}

	for (size_t i = 0; i < amountOfEquations; ++i) {
		QObject* damageItem = rootItem->findChild<QObject*>(QString("damage%1").arg(i));
		if (!damageItem) continue;

		QString damageString = QString("%1 + <font color='#ffda6502'>%2</font>")
			.arg(character.attackPower).arg(equations[i].damage);
		damageItem->setProperty("value", damageString);		
	}

	for (size_t i = 0; i < amountOfEquations; ++i) {
		QObject* defenceItem = rootItem->findChild<QObject*>(QString("defence%1").arg(i));
		if (!defenceItem) continue;

		int32_t defenceValue = equations[i].defence;
		if (defenceValue > 0) {
			QString defenceString = QString("%1 + <font color='#ff0265da'>%2</font>")
				.arg(character.defence).arg(defenceValue);
			defenceItem->setProperty("visible", true);
			defenceItem->setProperty("value", defenceString);
		}
		else {
			defenceItem->setProperty("visible", false);
		}
	}
}
void AetherEngine::updateInGameOverlay() {
	if (!inGameOverlay.renderer) return;

	auto rootItem = inGameOverlay.renderer->getRootItem();
	if (!rootItem) return;

	QObject* playerHealth = rootItem->findChild<QObject*>("playerHealth");
	QObject* playerDamage = rootItem->findChild<QObject*>("playerDamage");
	QObject* playerDefense = rootItem->findChild<QObject*>("playerDefense");
	QObject* playerExperience = rootItem->findChild<QObject*>("playerExperience");
	if (playerHealth)  { playerHealth->setProperty("value", character.health); }
	if (playerDamage)  { playerDamage->setProperty("value", character.attackPower); }
	if (playerDefense) { playerDefense->setProperty("value", character.defence); }
	if (playerExperience) { playerExperience->setProperty("value", character.experience); }

	QObject* mobTitle = rootItem->findChild<QObject*>("mobTitle");
	QObject* mobHealth = rootItem->findChild<QObject*>("mobHealth");
	QObject* mobDamage = rootItem->findChild<QObject*>("mobDamage");
	QObject* mobDefense = rootItem->findChild<QObject*>("mobDefense");
	QObject* mobExperience = rootItem->findChild<QObject*>("mobExperience");

	if (!gameContext.currentRoom->mobs.empty()) {
		Mob& mob = gameContext.currentRoom->mobs[0];
		if (mobTitle)      { mobTitle->setProperty("value", mob.id); }
		if (mobHealth)     { mobHealth->setProperty("value", mob.health); }
		if (mobDamage)     { mobDamage->setProperty("value", mob.attackPower); }
		if (mobDefense)    { mobDefense->setProperty("value", mob.defence); }
		if (mobExperience) { mobExperience->setProperty("value", mob.experienceReward); }
	}
	else {
		mobHealth->setProperty("value", 0); // making invisible
	}
}

void AetherEngine::renderQmlToTexture(UserInterfaceRenderer* renderer, Texture& texture)
{
	renderer->render();
	QImage image = renderer->getFbo()->toImage().convertToFormat(QImage::Format_RGBA8888);

	/*static bool hasSavedPauseMenuImage = false;

	if (!hasSavedPauseMenuImage && !image.isNull()) {
		image.save("pause_menu_debug.png");
		hasSavedPauseMenuImage = true;
	}*/

	if (texture.width != static_cast<uint32_t>(image.width()) ||
		texture.height != static_cast<uint32_t>(image.height())) {
		cleanupTexture(texture);

		createSolidColorTexture({ 0, 0, 0, 0 }, image.width(), image.height(), texture);
	}

	uploadRawDataToTexture(image.bits(), image.width(), image.height(), texture);
}

void AetherEngine::recordModelsToCommandBuffer(const std::vector<Model>& models, VkCommandBuffer commandBuffer)
{
	for (const Model& model : models) {
		recordModelToCommandBuffer(model, commandBuffer);
	}
}
void AetherEngine::recordModelToCommandBuffer(const Model& model, VkCommandBuffer commandBuffer)
{
	for (const Mesh& mesh : model.meshes) {
		const Material& material = mesh.material;
		const VkDescriptorSet& meshDescriptorSet = mesh.descriptorSets[currentFrame];

		if (mesh.UBOBuffers[currentFrame] != VK_NULL_HANDLE) {
			addBufferToDescriptorSet(
				meshDescriptorSet,
				mesh.UBOBuffers[currentFrame],
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				sizeof(UniformBufferObject), 0
			);
		}

		if (mesh.bones.size() > 0) {
			// adding bone data
			addBufferToDescriptorSet(
				meshDescriptorSet,
				mesh.boneSSBOBuffers[currentFrame],
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				sizeof(glm::mat4) * MAX_BONES_NUM, 1
			);
		}

		addTextureIfExistToDescriptorSet(material.diffuseTexture, meshDescriptorSet, 2);
		addTextureIfExistToDescriptorSet(material.emissiveTexture, meshDescriptorSet, 3);

		// Bind per-mesh descriptor set
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0, 1, &meshDescriptorSet,
			0, nullptr
		);

		bindVertexAndIndexBuffersToCommandBuffer(mesh, commandBuffer);
	}
}

void AetherEngine::createDescriptorPool(size_t modelsNum, size_t meshesNum, VkDescriptorPool& descriptorPool)
{
	std::array<VkDescriptorPoolSize, 3> poolSizes{};

	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = (meshesNum * 5 + modelsNum * 6)
		* static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) + 5;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[1].descriptorCount = (modelsNum * 4)
		* static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[2].descriptorCount = (meshesNum + 10)
		* static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = (meshesNum + modelsNum * 2 + 10)
		* static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	//std::cout << "totalMeshDescriptors: " << totalMeshDescriptors << "\n";
	//std::cout << "poolSizes[0].descriptorCount: " << poolSizes[0].descriptorCount << "\n";
	//std::cout << "poolInfo.maxSets: " << poolInfo.maxSets << "\n";

	if (vkCreateDescriptorPool(vkInit.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}
void AetherEngine::createDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout) const
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	// Bone UBO Binding
	VkDescriptorSetLayoutBinding boneUBOLayoutBinding{};
	boneUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	boneUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	boneUBOLayoutBinding.binding = 1;
	boneUBOLayoutBinding.descriptorCount = 1;
	boneUBOLayoutBinding.pImmutableSamplers = nullptr; // Optional

	// Diffuse Texture Binding
	VkDescriptorSetLayoutBinding diffuseSamplerLayoutBinding{};
	diffuseSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	diffuseSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	diffuseSamplerLayoutBinding.binding = 2;
	diffuseSamplerLayoutBinding.descriptorCount = 1;
	diffuseSamplerLayoutBinding.pImmutableSamplers = nullptr; // Optional

	// Emissive Texture Binding
	VkDescriptorSetLayoutBinding emissiveSamplerLayoutBinding{};
	emissiveSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	emissiveSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	emissiveSamplerLayoutBinding.binding = 3;
	emissiveSamplerLayoutBinding.descriptorCount = 1;
	emissiveSamplerLayoutBinding.pImmutableSamplers = nullptr; // Optional

	// Combine all bindings
	std::array<VkDescriptorSetLayoutBinding, 4> bindings = {
		uboLayoutBinding,
		boneUBOLayoutBinding,
		diffuseSamplerLayoutBinding,
		emissiveSamplerLayoutBinding
	};

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(vkInit.device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}
void AetherEngine::addTextureToDescriptorSet(
	const Texture& texture,
	VkDescriptorSet descriptorSet, uint32_t dstBinding
) const
{
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = texture.imageView;
	imageInfo.sampler = texture.sampler;

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = dstBinding;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(vkInit.device, 1, &descriptorWrite, 0, nullptr);
}
void AetherEngine::addTextureIfExistToDescriptorSet(
	const Texture& texture,
	VkDescriptorSet descriptorSet, uint32_t dstBinding
) const
{
	if (texture) {
		addTextureToDescriptorSet(
			texture,
			descriptorSet, dstBinding
		);
	}
	else {
		addTextureToDescriptorSet(
			transparentTexture,
			descriptorSet, dstBinding
		);
	}
}
void AetherEngine::addBufferToDescriptorSet(
	VkDescriptorSet descriptorSet, VkBuffer buffer,
	VkDescriptorType descriptorType,
	size_t bufferSize, uint32_t dstBinding
) const
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = bufferSize;

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.descriptorType = descriptorType;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = dstBinding;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(vkInit.device, 1, &descriptorWrite, 0, nullptr);
}

void AetherEngine::createDescriptorSets(std::vector<Model>& models, size_t swapchainImageCount)
{
	for (Model& model : models) {
		createDescriptorSets(model, swapchainImageCount);
	}
}
void AetherEngine::createDescriptorSets(Model& model, size_t swapchainImageCount)
{
	for (Mesh& mesh : model.meshes) {
		createDescriptorSets(mesh, swapchainImageCount);
	}
}
void AetherEngine::createDescriptorSets(Mesh& mesh, size_t swapchainImageCount)
{
	for (size_t frameIndex = 0; frameIndex < swapchainImageCount; ++frameIndex) {
		createDescriptorSet(mesh.descriptorSets[frameIndex]);
	}
}
void AetherEngine::createDescriptorSet(VkDescriptorSet& descriptorSet)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;  // Make sure you have a valid descriptor pool
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;  // Ensure the layout matches shader buffer binding requirements

	if (vkAllocateDescriptorSets(vkInit.device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor set!");
	}
}

/*void AetherEngine::bindVertexAndIndexBuffersToCommandBuffer(const Model& model, VkCommandBuffer commandBuffer)
{
	for (const Mesh& mesh : model.meshes) {
		bindVertexAndIndexBuffersToCommandBuffer(mesh, commandBuffer);
	}
}*/
void AetherEngine::bindVertexAndIndexBuffersToCommandBuffer(const Mesh& mesh, VkCommandBuffer commandBuffer)
{
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mesh.vertexBuffer, offsets);
	vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
}
