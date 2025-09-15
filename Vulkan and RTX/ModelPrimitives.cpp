#include "pch.h"
#include "ModelPrimitives.h"

std::vector<Model> ModelPrimitives::generateCubicLandscape(
	size_t landscapeWidth, size_t landscapeLenght,
	float cubeSize,
	glm::vec3 color,
	Texture& texture,
	ModelType modelType
)
{
	std::vector<Model> models;
	for (size_t i = 0; i < landscapeWidth; i++) {
		for (size_t j = 0; j < landscapeLenght; j++) {
			float random_height = 0.01 * (rand() % 51);
			models.push_back(createCube(
				0.0f + (float)i * cubeSize - landscapeWidth / 4,
				-2 + random_height,
				0.0f + (float)j * cubeSize - landscapeLenght / 4,
				cubeSize,
				color,
				texture,
				modelType
			));
		}
	}
	return models;
}
Model ModelPrimitives::createCube(
	float x, float y, float z,
	float cubeSize,
	glm::vec3 color,
	Texture& texture,
	ModelType modelType
)
{
	return createCuboid(
		x, y, z,
		cubeSize, cubeSize, cubeSize,
		color,
		texture,
		modelType
	);
}
Model ModelPrimitives::createCuboid(
	float x, float y, float z,
	float width, float height, float length,
	glm::vec3 color,
	Texture& texture,
	ModelType modelType
)
{
	std::vector<Vertex> localVertices(24);
	Model model{};
	model.type = modelType;
	Mesh mesh{};

	Material material{};
	material.diffuseTexture = texture;

#pragma region
	localVertices[0].position = { glm::vec3(0.0f , 0.0f  , 0.0f) + glm::vec3(x, y, z) };
	localVertices[1].position = { glm::vec3(width, 0.0f  , 0.0f) + glm::vec3(x, y, z) };
	localVertices[2].position = { glm::vec3(width, height, 0.0f) + glm::vec3(x, y, z) };
	localVertices[3].position = { glm::vec3(0.0f , height, 0.0f) + glm::vec3(x, y, z) };
	localVertices[4].position = { glm::vec3(0.0f , 0.0f  , length) + glm::vec3(x, y, z) };
	localVertices[5].position = { glm::vec3(width, 0.0f  , length) + glm::vec3(x, y, z) };
	localVertices[6].position = { glm::vec3(width, height, length) + glm::vec3(x, y, z) };
	localVertices[7].position = { glm::vec3(0.0f , height, length) + glm::vec3(x, y, z) };

	localVertices[8].position = { glm::vec3(0.0f , 0.0f  , 0.0f) + glm::vec3(x, y, z) };
	localVertices[9].position = { glm::vec3(width, 0.0f  , 0.0f) + glm::vec3(x, y, z) };
	localVertices[10].position = { glm::vec3(width, height, 0.0f) + glm::vec3(x, y, z) };
	localVertices[11].position = { glm::vec3(0.0f , height, 0.0f) + glm::vec3(x, y, z) };
	localVertices[12].position = { glm::vec3(0.0f , 0.0f  , length) + glm::vec3(x, y, z) };
	localVertices[13].position = { glm::vec3(width, 0.0f  , length) + glm::vec3(x, y, z) };
	localVertices[14].position = { glm::vec3(width, height, length) + glm::vec3(x, y, z) };
	localVertices[15].position = { glm::vec3(0.0f , height, length) + glm::vec3(x, y, z) };

	localVertices[16].position = { glm::vec3(0.0f , 0.0f  , 0.0f) + glm::vec3(x, y, z) };
	localVertices[17].position = { glm::vec3(width, 0.0f  , 0.0f) + glm::vec3(x, y, z) };
	localVertices[18].position = { glm::vec3(width, height, 0.0f) + glm::vec3(x, y, z) };
	localVertices[19].position = { glm::vec3(0.0f , height, 0.0f) + glm::vec3(x, y, z) };
	localVertices[20].position = { glm::vec3(0.0f , 0.0f  , length) + glm::vec3(x, y, z) };
	localVertices[21].position = { glm::vec3(width, 0.0f  , length) + glm::vec3(x, y, z) };
	localVertices[22].position = { glm::vec3(width, height, length) + glm::vec3(x, y, z) };
	localVertices[23].position = { glm::vec3(0.0f , height, length) + glm::vec3(x, y, z) };
#pragma endregion // pos
#pragma region
	localVertices[0].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[1].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[2].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[3].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[4].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };
	localVertices[5].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };
	localVertices[6].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };
	localVertices[7].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };

	localVertices[8].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[9].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[10].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };
	localVertices[11].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };
	localVertices[12].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[13].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[14].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };
	localVertices[15].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };

	localVertices[16].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
	localVertices[17].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[18].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[19].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
	localVertices[20].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
	localVertices[21].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[22].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[23].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
#pragma endregion // normal
#pragma region
	for (size_t i = 0; i < localVertices.size(); ++i) {
		localVertices[i].color = color;
	}
#pragma endregion // color
#pragma region
	localVertices[0].texCoord0 = { glm::vec2(0.0f, 0.0f) };
	localVertices[1].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[2].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[3].texCoord0 = { glm::vec2(0.0f, 1.0f) };
	localVertices[4].texCoord0 = { glm::vec2(0.0f, 1.0f) };
	localVertices[5].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[6].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[7].texCoord0 = { glm::vec2(0.0f, 0.0f) };

	localVertices[8].texCoord0 = { glm::vec2(0.0f, 1.0f) };
	localVertices[9].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[10].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[11].texCoord0 = { glm::vec2(0.0f, 0.0f) };
	localVertices[12].texCoord0 = { glm::vec2(0.0f, 0.0f) };
	localVertices[13].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[14].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[15].texCoord0 = { glm::vec2(0.0f, 1.0f) };

	localVertices[16].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[17].texCoord0 = { glm::vec2(0.0f, 0.0f) };
	localVertices[18].texCoord0 = { glm::vec2(0.0f, 1.0f) };
	localVertices[19].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[20].texCoord0 = { glm::vec2(0.0f, 0.0f) };
	localVertices[21].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[22].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[23].texCoord0 = { glm::vec2(0.0f, 1.0f) };
#pragma endregion // texCoord

	std::vector<uint32_t> localIndices = {
		0, 3, 1,
		1, 3, 2,
		4, 5, 7,
		7, 5, 6,

		11, 15, 10,
		10, 15, 14,
		12, 8, 13,
		13, 8, 9,

		20, 23, 16,
		16, 23, 19,
		17, 18, 21,
		21, 18, 22
	};

	// indices and vertices push back
	for (size_t i = 0; i < localVertices.size(); i++) {
		mesh.vertices.push_back(localVertices[i]);
	}
	for (size_t i = 0; i < localIndices.size(); i++) {
		mesh.indices.push_back(localIndices[i]);
	}

	mesh.material = material;

	model.meshes.push_back(mesh);
	//model.isCollidable = true;
	return model;
}
Model ModelPrimitives::createQuad(
	glm::vec3 origin,
	glm::vec2 size,
	glm::vec3 normal,
	glm::vec3 tangent,
	glm::vec3 color,
	Texture& texture
)
{
	glm::vec3 bitangent = glm::cross(normal, tangent);

	std::vector<Vertex> vertices(4);
	Model model{};
	Mesh mesh{};
	Material material{};
	material.diffuseTexture = texture;
	//material.diffuseTexture.hash = material.diffuseTexture.randomHash64();

	// Quad corners in tangent space
	/*vertices[0].position = origin;
	vertices[1].position = origin + tangent * size.x;
	vertices[2].position = origin + tangent * size.x + bitangent * size.y;
	vertices[3].position = origin + bitangent * size.y;*/

	vertices[0].position = origin;
	vertices[1].position = origin + glm::vec3(size.x, 0.0f, 0.0f);
	vertices[2].position = origin + glm::vec3(size.x, size.y, 0.0f);
	vertices[3].position = origin + glm::vec3(0.0f, size.y, 0.0f);

	// Set attributes
	for (size_t i = 0; i < 4; ++i) {
		vertices[i].normal = normal;
		vertices[i].color = color;
	}

	// Texcoords (standard winding)
	vertices[0].texCoord0 = { 0.0f, 0.0f };
	vertices[1].texCoord0 = { 1.0f, 0.0f };
	vertices[2].texCoord0 = { 1.0f, 1.0f };
	vertices[3].texCoord0 = { 0.0f, 1.0f };

	std::vector<uint32_t> indices = {
		0, 3, 1,
		1, 3, 2
	};

	mesh.vertices = std::move(vertices);
	mesh.indices = std::move(indices);
	mesh.material = material;

	model.meshes.push_back(std::move(mesh));
	return model;
}
void ModelPrimitives::createSkyModel(Model& model)
{
	std::vector<Vertex> localVertices(24);
	Mesh mesh;

#pragma region
	localVertices[0].position = { glm::vec3(-1.0f, -1.0f, -1.0f) };
	localVertices[1].position = { glm::vec3(1.0f, -1.0f, -1.0f) };
	localVertices[2].position = { glm::vec3(1.0f,  1.0f, -1.0f) };
	localVertices[3].position = { glm::vec3(-1.0f,  1.0f, -1.0f) };
	localVertices[4].position = { glm::vec3(-1.0f, -1.0f,  1.0f) };
	localVertices[5].position = { glm::vec3(1.0f, -1.0f,  1.0f) };
	localVertices[6].position = { glm::vec3(1.0f,  1.0f,  1.0f) };
	localVertices[7].position = { glm::vec3(-1.0f,  1.0f,  1.0f) };

	localVertices[8].position = { glm::vec3(-1.0f, -1.0f, -1.0f) };
	localVertices[9].position = { glm::vec3(1.0f, -1.0f, -1.0f) };
	localVertices[10].position = { glm::vec3(1.0f,  1.0f, -1.0f) };
	localVertices[11].position = { glm::vec3(-1.0f,  1.0f, -1.0f) };
	localVertices[12].position = { glm::vec3(-1.0f, -1.0f,  1.0f) };
	localVertices[13].position = { glm::vec3(1.0f, -1.0f,  1.0f) };
	localVertices[14].position = { glm::vec3(1.0f,  1.0f,  1.0f) };
	localVertices[15].position = { glm::vec3(-1.0f,  1.0f,  1.0f) };

	localVertices[16].position = { glm::vec3(-1.0f, -1.0f, -1.0f) };
	localVertices[17].position = { glm::vec3(1.0f, -1.0f, -1.0f) };
	localVertices[18].position = { glm::vec3(1.0f,  1.0f, -1.0f) };
	localVertices[19].position = { glm::vec3(-1.0f,  1.0f, -1.0f) };
	localVertices[20].position = { glm::vec3(-1.0f, -1.0f,  1.0f) };
	localVertices[21].position = { glm::vec3(1.0f, -1.0f,  1.0f) };
	localVertices[22].position = { glm::vec3(1.0f,  1.0f,  1.0f) };
	localVertices[23].position = { glm::vec3(-1.0f,  1.0f,  1.0f) };
#pragma endregion // pos
#pragma region
	localVertices[0].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[1].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[2].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[3].normal = { glm::vec3(0.0f, 0.0f, -1.0f) };
	localVertices[4].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };
	localVertices[5].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };
	localVertices[6].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };
	localVertices[7].normal = { glm::vec3(0.0f, 0.0f, 1.0f) };

	localVertices[8].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[9].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[10].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };
	localVertices[11].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };
	localVertices[12].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[13].normal = { glm::vec3(0.0f, -1.0f, 0.0f) };
	localVertices[14].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };
	localVertices[15].normal = { glm::vec3(0.0f, 1.0f, 0.0f) };

	localVertices[16].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
	localVertices[17].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[18].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[19].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
	localVertices[20].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
	localVertices[21].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[22].normal = { glm::vec3(1.0f, 0.0f, 0.0f) };
	localVertices[23].normal = { glm::vec3(-1.0f, 0.0f, 0.0f) };
#pragma endregion // normal
#pragma region
	for (size_t i = 0; i < localVertices.size(); ++i) {
		localVertices[i].color = glm::vec3(1.0f);
	}
#pragma endregion // color
#pragma region
	localVertices[0].texCoord0 = { glm::vec2(0.0f, 0.0f) };
	localVertices[1].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[2].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[3].texCoord0 = { glm::vec2(0.0f, 1.0f) };
	localVertices[4].texCoord0 = { glm::vec2(0.0f, 1.0f) };
	localVertices[5].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[6].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[7].texCoord0 = { glm::vec2(0.0f, 0.0f) };

	localVertices[8].texCoord0 = { glm::vec2(0.0f, 1.0f) };
	localVertices[9].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[10].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[11].texCoord0 = { glm::vec2(0.0f, 0.0f) };
	localVertices[12].texCoord0 = { glm::vec2(0.0f, 0.0f) };
	localVertices[13].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[14].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[15].texCoord0 = { glm::vec2(0.0f, 1.0f) };

	localVertices[16].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[17].texCoord0 = { glm::vec2(0.0f, 0.0f) };
	localVertices[18].texCoord0 = { glm::vec2(0.0f, 1.0f) };
	localVertices[19].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[20].texCoord0 = { glm::vec2(0.0f, 0.0f) };
	localVertices[21].texCoord0 = { glm::vec2(1.0f, 0.0f) };
	localVertices[22].texCoord0 = { glm::vec2(1.0f, 1.0f) };
	localVertices[23].texCoord0 = { glm::vec2(0.0f, 1.0f) };
#pragma endregion // texCoord

	std::vector<uint32_t> localIndices = {
		1, 3, 0,
		2, 3, 1,
		7, 5, 4,
		6, 5, 7,

		10, 15, 11,
		14, 15, 10,
		13, 8, 12,
		9, 8, 13,

		16, 23, 20,
		19, 23, 16,
		21, 18, 17,
		22, 18, 21
	};

	// indices and vertices push back
	for (size_t i = 0; i < localVertices.size(); i++) {
		mesh.vertices.push_back(localVertices[i]);
	}
	for (size_t i = 0; i < localIndices.size(); i++) {
		mesh.indices.push_back(localIndices[i]);
	}
	model.meshes.push_back(mesh);
}