#pragma once

#include "Model.h"

namespace ModelManager
{
	std::vector<Model> generateCubicLandscape(
		size_t landscapeWidth, size_t landscapeLenght,
		float_t cubeSize,
		glm::vec3 color,
		Texture& texture,
		ModelType modelType
	);
	Model createCube(
		float x, float y, float z, float cubeSize,
		glm::vec3 color,
		Texture& texture,
		ModelType modelType
	);
	Model createCuboid(
		float x, float y, float z,
		float width, float height, float length,
		glm::vec3 color,
		Texture& texture,
		ModelType modelType
	);
	Model createQuad(
		glm::vec3 origin,
		glm::vec2 size,
		glm::vec3 normal,
		glm::vec3 tangent,
		glm::vec3 color,
		Texture& texture
	);

	void createSkyModel(Model& model);

	void loadObjModel(const std::string& filePath, std::vector<Model>& models);
	void loadGltfModel(const std::string& filePath, std::vector<Model>& models);
}