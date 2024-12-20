#include "pch.h"
#include "VulkanAndRTX.h"
#include "StellarCalculations.h"

// information about framebuffer attachments, how many color and depth buffers there will
// be, how many samples to use for each of them and how their contents should be treated
void VulkanAndRTX::createRenderPass(VkRenderPass& renderPass)
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
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

void VulkanAndRTX::createPipelineLayout(VkDescriptorSetLayout& descriptorSetLayout)
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

void VulkanAndRTX::createGraphicsPipeline(
	const std::string prefix, 
	const std::string& vertexShader, const std::string& fragmentShader
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
	multisampling.rasterizationSamples = vkInit.colorSamples;
	multisampling.minSampleShading = 1.0f; // Optional

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = (prefix == "sky" ? VK_FALSE : VK_TRUE);
	depthStencil.depthWriteEnable = (prefix == "sky" ? VK_FALSE : VK_TRUE);
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

	// altering variables for shaders without their recreation
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();
#pragma endregion

	createPipelineLayout(descriptorSetLayout);

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
	pipelineInfo.renderPass = objectRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
#pragma endregion

	VkPipeline pipeline{};
	if (vkCreateGraphicsPipelines(vkInit.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
		std::cout << prefix << " ";
		throw std::runtime_error("failed to create graphics pipeline!");
	}
	pipelines[prefix] = pipeline;

	vkDestroyShaderModule(vkInit.device, fragShaderModule, nullptr);
	vkDestroyShaderModule(vkInit.device, vertShaderModule, nullptr);
}

// Creating frames for presentation
void VulkanAndRTX::drawFrame(float timeSinceLaunch, ImDrawData* draw_data)
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

	updateUniformBuffers(currentFrame, timeSinceLaunch);

	// Only reset the fence if we are submitting work
	vkResetFences(vkInit.device, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex, draw_data);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
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

void VulkanAndRTX::updateUniformBuffers(uint32_t currentImage, float timeSinceLaunch)
{
	glm::mat4 view = glm::lookAt(
		inputHandler.camera.getLookFrom(),
		inputHandler.camera.getLookFrom() + inputHandler.camera.getDirection(),
		inputHandler.camera.getVerticalWorldAxis()
	);

	glm::mat4 projection = glm::perspective(
		glm::radians(inputHandler.camera.getVerticalFov()),
		swapChainExtent.width / (float)swapChainExtent.height, 
		0.01f, 100000.0f
	);
	projection[1][1] *= -1;

	glm::vec3 sun = StellarCalculations::calculateSunPosition(timeSinceLaunch);

	// add transpose(inverse(ubo.model)) if doing non uniform scaling

	skyUBO.model = glm::mat4(glm::mat3(view));
	skyUBO.view = view;
	skyUBO.projection = projection;
	skyUBO.sun = sun;
	skyUBO.observer = inputHandler.camera.getLookFrom();

	void* data;
	vkMapMemory(vkInit.device, skyUniformBuffersMemory[currentImage],
		0, sizeof(UniformBufferObject), 0, &data);
	memcpy(data, &skyUBO, sizeof(UniformBufferObject));
	vkUnmapMemory(vkInit.device, skyUniformBuffersMemory[currentImage]);

	// Update per-mesh shader buffers
	for (size_t modelIndex = 0; modelIndex < models.size(); ++modelIndex) {
		Model& model = models[modelIndex];

		for (size_t meshIndex = 0; meshIndex < model.meshes.size(); ++meshIndex) {
			Mesh& mesh = model.meshes[meshIndex];

			// update bones
			if (model.bones.size() > 0) {
				for (size_t i = 0; i < model.bones.size(); i++) {
					boneSSBO.boneTransforms[i] = model.bones[i].finalTransform;
					//std::cout << glm::to_string(boneUBO.boneTransforms[i]) << "\n";
				}
			}
			else {
				for (size_t i = 0; i < model.bones.size(); i++) {
					boneSSBO.boneTransforms[i] = glm::mat4(1.0);
				}
			}

			vkMapMemory(vkInit.device, boneUniformBuffersMemory[modelIndex][meshIndex][currentImage],
						0, sizeof(glm::mat4) * BONES_NUM, 0, &data);
			memcpy(data, boneSSBO.boneTransforms.data(), sizeof(glm::mat4) * BONES_NUM);

			/*glm::mat4* mappedMatrices = reinterpret_cast<glm::mat4*>(data);
			for (size_t i = 0; i < BONES_NUM; ++i) {
				std::cout << "Matrix " << i << ": " << glm::to_string(mappedMatrices[i]) << std::endl;
			}*/

			vkUnmapMemory(vkInit.device, boneUniformBuffersMemory[modelIndex][meshIndex][currentImage]);

			//meshUBO.model = mesh.transform;
			meshUBO.model = glm::mat4(1.0);
			meshUBO.view = view;
			meshUBO.projection = projection;
			meshUBO.sun = sun;
			meshUBO.observer = inputHandler.camera.getLookFrom();

			vkMapMemory(vkInit.device,
				meshUniformBuffersMemory[modelIndex][meshIndex][currentImage],
				0, sizeof(UniformBufferObject), 0, &data);
			memcpy(data, &meshUBO, sizeof(UniformBufferObject));
			vkUnmapMemory(vkInit.device,
				meshUniformBuffersMemory[modelIndex][meshIndex][currentImage]);
		}
	}
}
void VulkanAndRTX::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, ImDrawData* draw_data)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = objectRenderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	// sky
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines["sky"]);
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0, 1, &skyDescriptorSets[currentFrame],
			0, nullptr
		);

		drawModel(commandBuffer, sky);
	}

	// objects
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines["object"]);
	for (size_t modelIndex = 0; modelIndex < models.size(); ++modelIndex) {
		for (size_t meshIndex = 0; meshIndex < models[modelIndex].meshes.size(); ++meshIndex) {
			const Mesh& mesh = models[modelIndex].meshes[meshIndex];
			const Material& material = models[modelIndex].materials[meshIndex];

			if (material.diffuseTexture.imageView && material.diffuseTexture.sampler) {
				addTextureToDescriptorSet(
					material.diffuseTexture,
					meshDescriptorSets[modelIndex][meshIndex][currentFrame], 2
				);
			}
			else {
				addTextureToDescriptorSet(
					dummyTexture,
					meshDescriptorSets[modelIndex][meshIndex][currentFrame], 2
				);
			}
			if (material.emissiveTexture.imageView && material.emissiveTexture.sampler) {
				addTextureToDescriptorSet(
					material.emissiveTexture,
					meshDescriptorSets[modelIndex][meshIndex][currentFrame], 3
				);
			}
			else {
				addTextureToDescriptorSet(
					dummyTexture,
					meshDescriptorSets[modelIndex][meshIndex][currentFrame], 3
				);
			}

			//if (models[modelIndex].meshes[meshIndex].bones.size() > 0) {
				// adding bone data
			addBufferToDescriptorSet(
				meshDescriptorSets[modelIndex][meshIndex][currentFrame],
				boneUniformBuffers[modelIndex][meshIndex][currentFrame],
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				sizeof(glm::mat4) * BONES_NUM, 1
			);
			//}

			// Bind per-mesh descriptor set
			vkCmdBindDescriptorSets(
				commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS, 
				pipelineLayout,
				0, 1, &meshDescriptorSets[modelIndex][meshIndex][currentFrame],
				0, nullptr
			);

			drawMesh(commandBuffer, mesh);
		}
	}

	ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffers[currentFrame]);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void VulkanAndRTX::createDescriptorPool()
{
	size_t totalMeshes = 0;
	for (const auto& model : models) {
		totalMeshes += model.meshes.size();
	}

	std::array<VkDescriptorPoolSize, 3> poolSizes{};

	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = (totalMeshes * 5 + models.size() * 6)
		* static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) + 5;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[1].descriptorCount = (models.size() * 4)
		* static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[2].descriptorCount = (totalMeshes + 2)
		* static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = (totalMeshes + models.size() * 2 + 2)
		* static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	//std::cout << "totalMeshDescriptors: " << totalMeshDescriptors << "\n";
	//std::cout << "poolSizes[0].descriptorCount: " << poolSizes[0].descriptorCount << "\n";
	//std::cout << "poolInfo.maxSets: " << poolInfo.maxSets << "\n";

	if (vkCreateDescriptorPool(vkInit.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}
void VulkanAndRTX::createDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout) const
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
void VulkanAndRTX::createDescriptorSet(
	VkDescriptorSet& descriptorSet,
	VkBuffer buffer, VkDescriptorType descriptorType, 
	size_t bufferSize, uint32_t dstBinding
)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;  // Make sure you have a valid descriptor pool
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;  // Ensure the layout matches UBO binding requirements

	if (vkAllocateDescriptorSets(vkInit.device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor set!");
	}

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
void VulkanAndRTX::addTextureToDescriptorSet(
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
void VulkanAndRTX::addBufferToDescriptorSet(
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
void VulkanAndRTX::createSkyUniformBuffers(size_t swapChainImageCount)
{
	skyUniformBuffers.resize(swapChainImageCount);
	skyUniformBuffersMemory.resize(swapChainImageCount);

	for (size_t i = 0; i < swapChainImageCount; i++) {
		createBuffer(
			sizeof(UniformBufferObject),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			skyUniformBuffers[i],
			skyUniformBuffersMemory[i]
		);
	}
}
void VulkanAndRTX::createMeshShaderBuffers(size_t swapChainImageCount)
{
	meshUniformBuffers.resize(models.size());
	boneUniformBuffers.resize(models.size());
	meshUniformBuffersMemory.resize(models.size());
	boneUniformBuffersMemory.resize(models.size());

	for (size_t i = 0; i < BONES_NUM; ++i) {
		boneSSBO.boneTransforms[i] = glm::mat4(0.0f);
	}

	for (size_t modelIndex = 0; modelIndex < models.size(); ++modelIndex) {
		const Model& model = models[modelIndex];

		meshUniformBuffers[modelIndex].resize(model.meshes.size());
		boneUniformBuffers[modelIndex].resize(model.meshes.size());
		meshUniformBuffersMemory[modelIndex].resize(model.meshes.size());
		boneUniformBuffersMemory[modelIndex].resize(model.meshes.size());

		for (size_t meshIndex = 0; meshIndex < model.meshes.size(); ++meshIndex) {
			meshUniformBuffers[modelIndex][meshIndex].resize(swapChainImageCount);
			boneUniformBuffers[modelIndex][meshIndex].resize(swapChainImageCount);
			meshUniformBuffersMemory[modelIndex][meshIndex].resize(swapChainImageCount);
			boneUniformBuffersMemory[modelIndex][meshIndex].resize(swapChainImageCount);

			for (size_t frameIndex = 0; frameIndex < swapChainImageCount; ++frameIndex) {
				createBuffer(
					sizeof(UniformBufferObject),
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					meshUniformBuffers[modelIndex][meshIndex][frameIndex],
					meshUniformBuffersMemory[modelIndex][meshIndex][frameIndex]
				);

				createBuffer(
					sizeof(glm::mat4) * BONES_NUM,
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					boneUniformBuffers[modelIndex][meshIndex][frameIndex],
					boneUniformBuffersMemory[modelIndex][meshIndex][frameIndex]
				);
			}
		}
	}
}
void VulkanAndRTX::createSkyDescriptorSets(size_t swapChainImageCount)
{
	skyDescriptorSets.resize(swapChainImageCount);

	for (size_t i = 0; i < skyDescriptorSets.size(); i++) {
		createDescriptorSet(
			skyDescriptorSets[i],
			skyUniformBuffers[i],
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			sizeof(UniformBufferObject), 0
		);
	}
}
void VulkanAndRTX::createMeshDescriptorSets(size_t swapChainImageCount)
{
	meshDescriptorSets.resize(models.size());

	for (size_t modelIndex = 0; modelIndex < models.size(); ++modelIndex) {
		const Model& model = models[modelIndex];
		meshDescriptorSets[modelIndex].resize(model.meshes.size());

		for (size_t meshIndex = 0; meshIndex < model.meshes.size(); ++meshIndex) {
			meshDescriptorSets[modelIndex][meshIndex].resize(swapChainImageCount);

			for (size_t frameIndex = 0; frameIndex < swapChainImageCount; ++frameIndex) {
				createDescriptorSet(
					meshDescriptorSets[modelIndex][meshIndex][frameIndex],
					meshUniformBuffers[modelIndex][meshIndex][frameIndex],
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					sizeof(UniformBufferObject), 0
				);
			}
		}
	}
}

void VulkanAndRTX::drawModel(VkCommandBuffer commandBuffer, const Model& model)
{
	for (const Mesh& mesh : model.meshes) {
		drawMesh(commandBuffer, mesh);
	}
}
void VulkanAndRTX::drawMesh(VkCommandBuffer commandBuffer, const Mesh& mesh)
{
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mesh.vertexBuffer, offsets);
	vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);
}
