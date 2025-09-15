#pragma once

#include "pch.h"
#include "Model.h"
#include "VulkanInitializer.h"
#include "BufferManager.h"
#include "ImageManager.h"

class ModelManager
{
public:
	ModelManager(
		VulkanInitializer& vkInitRef,
		BufferManager& bufferManagerRef,
		ImageManager& imagerManagerRef
	);

	void loadModelsFromFolder(
		const std::string& directory,
		std::vector<Model>& models
	);
	Texture loadTextureForModel(const std::string& texturePath, const aiScene* scene);
	void loadTexturesFromFolder(
		const std::string& texturePath,
		std::unordered_map<std::string, Texture>& textures
	);
	Material processMaterial(aiMaterial* aiMat, const aiScene* scene);
	void processNode(
		aiNode* node, const aiScene* scene,
		std::vector<Model>& models,
		Model& parentModel,
		glm::mat4 parentTransform, const glm::mat4 globalInverseTransform,
		uint32_t& perModelVertexOffset,
		int level
	);

	void uploadRawDataToTexture(void* rawImage, uint32_t width, uint32_t height, Texture& texture);

	void createTextureFromPixelData(
		const void* pixelData,
		uint32_t texWidth,
		uint32_t texHeight,
		uint32_t mipLevels,
		Texture& texture
	);
	void createSolidColorTexture(
		std::array<uint8_t, 4> color, uint32_t width, uint32_t height, Texture& texture
	);
	Texture loadTextureFromPath(const std::string& texturePath);
	void createTextureFromEmbedded(
		const std::string& embeddedTextureName,
		const aiScene* scene, Texture& texture
	);

	// creating image for MSAA sampling
	void createColorTexture(VkFormat& imageFormat, uint32_t width, uint32_t height, Texture& texture);
	void createDepthTexture(uint32_t width, uint32_t height, Texture& texture);

private:
	VulkanInitializer& vkInit;
	BufferManager& bufferManager;
	ImageManager& imageManager;

	std::vector<std::string> GetModelFiles(const std::string& directory);
	glm::mat4 assimpToGLMMat4(const aiMatrix4x4& from);
	void decomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::quat& rotation, glm::vec3& scale);
	glm::mat4 setScaleToOne(const glm::mat4& matrix);
};