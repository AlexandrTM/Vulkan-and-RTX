#pragma once

#include "Model.h"

namespace ModelManager
{
	void generateCubicLandscape(
		size_t landscapeWidth, size_t landscapeLenght,
		float_t cubeSize,
		glm::vec3 color,
		Texture& texture,
		std::vector<Model>& models
	);
	void createCube(
		float x, float y, float z, float cubeSize,
		glm::vec3 color,
		Texture& texture,
		std::vector<Model>& models
	);
	void createCuboid(
		float x, float y, float z,
		float width, float height, float length,
		glm::vec3 color,
		Texture& texture,
		std::vector<Model>& models
	);

	void createSkyModel(Model& model);

	void loadObjModel(const std::string& filePath, std::vector<Model>& models);
	void loadGltfModel(const std::string& filePath, std::vector<Model>& models);
}