#include "pch.h"
#include "VulkanAndRTX.h"
#include "Vertex.h"

void VulkanAndRTX::generateTerrain(float startX, float startZ, size_t width, size_t length,
	float gridSize, float scale, float height, size_t seed)
{
	Model model;
	Mesh mesh;
	terrainGenerator = std::make_unique<TerrainGenerator>(seed);
	auto heightmap = terrainGenerator.get()->generatePerlinHeightMap(width, length, scale, height);
	terrainGenerator.get()->generateTerrainMesh(startX, startZ, heightmap, gridSize, mesh);
	model.meshes.push_back(mesh);
	modelsBuffer.models.push_back(model);
}

void VulkanAndRTX::generateCubicLandscape(size_t landscapeWidth, size_t landscapeLenght, float_t cubeSize)
{
	for (size_t i = 0; i < landscapeWidth; i++)
	{
		for (size_t j = 0; j < landscapeLenght; j++)
		{
			float random_height = 0.01 * (rand() % 51);
			generateCube(0.0f + (float)i * cubeSize - landscapeWidth / 4,
				-2 + random_height,
				0.0f + (float)j * cubeSize - landscapeLenght / 4,
				cubeSize);
		}
	}
}
void VulkanAndRTX::generateCube(float x, float y, float z, float cubeSize)
{
	glm::vec3 basicColor = glm::vec3(0.5f, 0.5f, 0.5f);

	std::vector<Vertex> localVertices(24);
	Model model;
	Mesh mesh;

#pragma region
	localVertices[0].position  = { glm::vec3(0.0f    , 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[1].position  = { glm::vec3(cubeSize, 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[2].position  = { glm::vec3(cubeSize, cubeSize, 0.0f)     + glm::vec3(x, y, z) };
	localVertices[3].position  = { glm::vec3(0.0f    , cubeSize, 0.0f)     + glm::vec3(x, y, z) };
	localVertices[4].position  = { glm::vec3(0.0f    , 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[5].position  = { glm::vec3(cubeSize, 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[6].position  = { glm::vec3(cubeSize, cubeSize, cubeSize) + glm::vec3(x, y, z) };
	localVertices[7].position  = { glm::vec3(0.0f    , cubeSize, cubeSize) + glm::vec3(x, y, z) };
	
	localVertices[8].position  = { glm::vec3(0.0f    , 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[9].position  = { glm::vec3(cubeSize, 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[10].position = { glm::vec3(cubeSize, cubeSize, 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[11].position = { glm::vec3(0.0f    , cubeSize, 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[12].position = { glm::vec3(0.0f    , 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[13].position = { glm::vec3(cubeSize, 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[14].position = { glm::vec3(cubeSize, cubeSize, cubeSize) + glm::vec3(x, y, z) };
	localVertices[15].position = { glm::vec3(0.0f    , cubeSize, cubeSize) + glm::vec3(x, y, z) };

	localVertices[16].position = { glm::vec3(0.0f    , 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[17].position = { glm::vec3(cubeSize, 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[18].position = { glm::vec3(cubeSize, cubeSize, 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[19].position = { glm::vec3(0.0f    , cubeSize, 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[20].position = { glm::vec3(0.0f    , 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[21].position = { glm::vec3(cubeSize, 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[22].position = { glm::vec3(cubeSize, cubeSize, cubeSize) + glm::vec3(x, y, z) };
	localVertices[23].position = { glm::vec3(0.0f    , cubeSize, cubeSize) + glm::vec3(x, y, z) };
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
	for (size_t i = 0; i < localVertices.size(); ++i)
	{
		localVertices[i].color = basicColor;
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
	for (size_t i = 0; i < localVertices.size(); i++)
	{
		mesh.vertices.push_back(localVertices[i]);
	}
	for (size_t i = 0; i < localIndices.size(); i++)
	{
		mesh.indices.push_back(localIndices[i]);
	}
	model.meshes.push_back(mesh);
	modelsBuffer.models.push_back(model);
}
void VulkanAndRTX::generateCuboid(float x, float y, float z,
	float width, float height, float length, glm::vec3 color)
{
	glm::vec3 basicColor = color;

	std::vector<Vertex> localVertices(24);
	Model model;
	Mesh mesh;

#pragma region
	localVertices[0].position = { glm::vec3(0.0f , 0.0f  , 0.0f) + glm::vec3(x, y, z) };
	localVertices[1].position = { glm::vec3(width, 0.0f  , 0.0f) + glm::vec3(x, y, z) };
	localVertices[2].position = { glm::vec3(width, height, 0.0f) + glm::vec3(x, y, z) };
	localVertices[3].position = { glm::vec3(0.0f , height, 0.0f) + glm::vec3(x, y, z) };
	localVertices[4].position = { glm::vec3(0.0f , 0.0f  , length) + glm::vec3(x, y, z) };
	localVertices[5].position = { glm::vec3(width, 0.0f  , length) + glm::vec3(x, y, z) };
	localVertices[6].position = { glm::vec3(width, height, length) + glm::vec3(x, y, z) };
	localVertices[7].position = { glm::vec3(0.0f , height, length) + glm::vec3(x, y, z) };

	localVertices[8].position  = { glm::vec3(0.0f , 0.0f  , 0.0f) + glm::vec3(x, y, z) };
	localVertices[9].position  = { glm::vec3(width, 0.0f  , 0.0f) + glm::vec3(x, y, z) };
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
	for (size_t i = 0; i < localVertices.size(); ++i)
	{
		localVertices[i].color = basicColor;
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
	for (size_t i = 0; i < localVertices.size(); i++)
	{
		mesh.vertices.push_back(localVertices[i]);
	}
	for (size_t i = 0; i < localIndices.size(); i++)
	{
		mesh.indices.push_back(localIndices[i]);
	}
	model.meshes.push_back(mesh);
	modelsBuffer.models.push_back(model);
}
void VulkanAndRTX::createSkyCube()
{
	glm::vec3 basicColor = glm::vec3(0.5f, 0.5f, 0.5f);

	std::vector<Vertex> localVertices(24);
	Model model;
	Mesh mesh;
	
#pragma region
	localVertices[0].position  = { glm::vec3(-1.0f, -1.0f, -1.0f) };
	localVertices[1].position  = { glm::vec3( 1.0f, -1.0f, -1.0f) };
	localVertices[2].position  = { glm::vec3( 1.0f,  1.0f, -1.0f) };
	localVertices[3].position  = { glm::vec3(-1.0f,  1.0f, -1.0f) };
	localVertices[4].position  = { glm::vec3(-1.0f, -1.0f,  1.0f) };
	localVertices[5].position  = { glm::vec3( 1.0f, -1.0f,  1.0f) };
	localVertices[6].position  = { glm::vec3( 1.0f,  1.0f,  1.0f) };
	localVertices[7].position  = { glm::vec3(-1.0f,  1.0f,  1.0f) };

	localVertices[8].position  = { glm::vec3(-1.0f, -1.0f, -1.0f) };
	localVertices[9].position  = { glm::vec3( 1.0f, -1.0f, -1.0f) };
	localVertices[10].position = { glm::vec3( 1.0f,  1.0f, -1.0f) };
	localVertices[11].position = { glm::vec3(-1.0f,  1.0f, -1.0f) };
	localVertices[12].position = { glm::vec3(-1.0f, -1.0f,  1.0f) };
	localVertices[13].position = { glm::vec3( 1.0f, -1.0f,  1.0f) };
	localVertices[14].position = { glm::vec3( 1.0f,  1.0f,  1.0f) };
	localVertices[15].position = { glm::vec3(-1.0f,  1.0f,  1.0f) };

	localVertices[16].position = { glm::vec3(-1.0f, -1.0f, -1.0f) };
	localVertices[17].position = { glm::vec3( 1.0f, -1.0f, -1.0f) };
	localVertices[18].position = { glm::vec3( 1.0f,  1.0f, -1.0f) };
	localVertices[19].position = { glm::vec3(-1.0f,  1.0f, -1.0f) };
	localVertices[20].position = { glm::vec3(-1.0f, -1.0f,  1.0f) };
	localVertices[21].position = { glm::vec3( 1.0f, -1.0f,  1.0f) };
	localVertices[22].position = { glm::vec3( 1.0f,  1.0f,  1.0f) };
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
	for (size_t i = 0; i < localVertices.size(); ++i)
	{
		localVertices[i].color = basicColor;
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
	for (size_t i = 0; i < localVertices.size(); i++)
	{
		mesh.vertices.push_back(localVertices[i]);
	}
	for (size_t i = 0; i < localIndices.size(); i++)
	{
		mesh.indices.push_back(localIndices[i]);
	}
	model.meshes.push_back(mesh);
	modelsBuffer.sky = model;
}

void VulkanAndRTX::loadObjModel(const std::string& modelPath)
{
	Model model;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes) {
		Mesh mesh;

		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord0 = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1 - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.f, 1.f, 1.f };

			vertex.normal = {
				attrib.normals[index.normal_index * 3],
				attrib.normals[index.normal_index * 3 + 1],
				attrib.normals[index.normal_index * 3 + 2]
			};

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
				mesh.vertices.push_back(vertex);
			}

			mesh.indices.push_back(uniqueVertices[vertex]);
		}
		model.meshes.push_back(mesh);
	}
	modelsBuffer.models.push_back(model);
}
void VulkanAndRTX::loadGltfModel(const std::string& modelPath) {
	tinygltf::Model GLTFmodel;
	tinygltf::TinyGLTF loader;
	std::string error;
	std::string warning;

	Model model;

	bool binary = false;
	size_t extPos = modelPath.rfind('.', modelPath.length());
	if (extPos != std::string::npos) {
		binary = (modelPath.substr(extPos + 1, modelPath.length() - extPos - 1) == "glb");
	}

	bool result = binary ? loader.LoadBinaryFromFile(&GLTFmodel, &error, &warning, modelPath.c_str())
		: loader.LoadASCIIFromFile(&GLTFmodel, &error, &warning, modelPath.c_str());

	if (!result) {
		std::cout << "model not loaded: " + modelPath << "\n";
		std::cout << error << "\n";
	}

	for (const auto& GLTFmesh : GLTFmodel.meshes) {
		Mesh mesh;
		for (const auto& primitive : GLTFmesh.primitives) {
			size_t currentVertex = 0;
			size_t currentIndex = 0;

			const float* bufferPositions = nullptr;
			const float* bufferNormals = nullptr;
			const float* bufferTexCoordSet0 = nullptr;
			const float* bufferTexCoordSet1 = nullptr;
			const float* bufferColorSet0 = nullptr;
			const void* bufferJoints = nullptr;
			const float* bufferWeights = nullptr;

			const auto& attributes = primitive.attributes;

			// Extract POSITION
			if (attributes.find("POSITION") != attributes.end()) {
				const tinygltf::Accessor& positionAccessor = GLTFmodel.accessors[attributes.find("POSITION")->second];
				const tinygltf::BufferView& posView = GLTFmodel.bufferViews[positionAccessor.bufferView];
				bufferPositions = reinterpret_cast<const float*>(&(GLTFmodel.buffers[posView.buffer].data[positionAccessor.byteOffset + posView.byteOffset]));

				// normals
				if (attributes.find("NORMAL") != attributes.end()) {
					const tinygltf::Accessor& normalAccessor = GLTFmodel.accessors[attributes.find("NORMAL")->second];
					const tinygltf::BufferView& normalView = GLTFmodel.bufferViews[normalAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float*>(&(GLTFmodel.buffers[normalView.buffer].data[normalAccessor.byteOffset + normalView.byteOffset]));
				}

				// texCoords
				if (attributes.find("TEXCOORD_0") != attributes.end()) {
					const tinygltf::Accessor& texCoordsAccessor0 = GLTFmodel.accessors[attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& texCoordsView0 = GLTFmodel.bufferViews[texCoordsAccessor0.bufferView];
					bufferTexCoordSet0 = reinterpret_cast<const float*>(&(GLTFmodel.buffers[texCoordsView0.buffer].data[texCoordsAccessor0.byteOffset + texCoordsView0.byteOffset]));
				}
				if (attributes.find("TEXCOORD_1") != attributes.end()) {
					const tinygltf::Accessor& texCoordsAccessor1 = GLTFmodel.accessors[attributes.find("TEXCOORD_1")->second];
					const tinygltf::BufferView& texCoordsView1 = GLTFmodel.bufferViews[texCoordsAccessor1.bufferView];
					bufferTexCoordSet1 = reinterpret_cast<const float*>(&(GLTFmodel.buffers[texCoordsView1.buffer].data[texCoordsAccessor1.byteOffset + texCoordsView1.byteOffset]));
				}

				// Extract INDICES
				{
					const tinygltf::Accessor& indicesAccessor = GLTFmodel.accessors[primitive.indices > -1 ? primitive.indices : 0];
					const tinygltf::BufferView& indicesView = GLTFmodel.bufferViews[indicesAccessor.bufferView];
					const tinygltf::Buffer& buffer = GLTFmodel.buffers[indicesView.buffer];

					const unsigned char* indicesPreData =
						&(buffer.data[indicesAccessor.byteOffset + indicesView.byteOffset]);

					mesh.indices.resize(indicesAccessor.count);

					switch (indicesAccessor.componentType)
					{
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
						const uint8_t* indicesData = reinterpret_cast<const uint8_t*>(indicesPreData);

						for (size_t i = 0; i < indicesAccessor.count; i++) {
							mesh.indices[currentIndex] = indicesData[i] + currentVertex;
							currentIndex += 1;
						}
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
						const uint16_t* indicesData = reinterpret_cast<const uint16_t*>(indicesPreData);
						
						for (size_t i = 0; i < indicesAccessor.count; i++) {
							mesh.indices[currentIndex] = indicesData[i] + currentVertex;
							currentIndex += 1;
						}
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
						const uint32_t* indicesData = reinterpret_cast<const uint32_t*>(indicesPreData);

						for (size_t i = 0; i < indicesAccessor.count; i++) {
							mesh.indices[currentIndex] = indicesData[i] + currentVertex;
							currentIndex += 1;
						}
						break;
					}
					default:
						break;
					}
				}

				// Iterate through vertices
				for (size_t i = 0; i < positionAccessor.count; ++i) {
					Vertex vertex{};

					// Extract POSITION
					vertex.position = glm::vec3(
						bufferPositions[i * 3],
						bufferPositions[i * 3 + 1],
						bufferPositions[i * 3 + 2]);

					// Extract NORMAL
					vertex.normal = glm::normalize(glm::vec3(bufferNormals ? glm::vec3(
						bufferNormals[i * 3],
						bufferNormals[i * 3 + 1],
						bufferNormals[i * 3 + 2]) : glm::vec3(0.0f)));

					vertex.color = { 0.5f, 0.5f, 0.5f };

					vertex.texCoord0 = bufferTexCoordSet0 ? glm::vec2(
						bufferTexCoordSet0[i * 2],
						bufferTexCoordSet0[i * 2 + 1]) : glm::vec3(0.0f);
					vertex.texCoord1 = bufferTexCoordSet1 ? glm::vec2(
						bufferTexCoordSet1[i * 2],
						bufferTexCoordSet1[i * 2 + 1]) : glm::vec3(0.0f);

					mesh.vertices.push_back(vertex);
					currentVertex += 1;
				}
			}
		}
		std::cout << 
			"mesh name: " << GLTFmesh.name << " "
			"vertices count: " << mesh.vertices.size() << "\n";
		model.meshes.push_back(mesh);
	}
	modelsBuffer.models.push_back(model);
	std::cout << "textures: " << GLTFmodel.textures.size() << "\n";
}

static std::vector<std::string> GetModelFiles(const std::string& directory) {
	std::vector<std::string> modelFiles;

	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		if (entry.is_regular_file()) {
			const auto& path = entry.path();
			if (path.extension() == ".gltf" || path.extension() == ".glb") {
				modelFiles.push_back(path.string());
			}
		}
	}

	return modelFiles;
}
void VulkanAndRTX::createTextureImageFromPath(const std::string& texturePath, Texture& texture)
{
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;
	texture.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	if (!pixels) {
		std::cout << texturePath;
		throw std::runtime_error("failed to load texture image!");
	}

	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(vkInit.device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(vkInit.device, stagingBufferMemory);
	stbi_image_free(pixels);

	createImage(texWidth, texHeight, texture.mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
		| VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texture.image, texture.imageMemory);

	transitionImageLayout(texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture.mipLevels);

	copyBufferToImage(stagingBuffer, texture.image,
		static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	vkDestroyBuffer(vkInit.device, stagingBuffer, nullptr);
	vkFreeMemory(vkInit.device, stagingBufferMemory, nullptr);

	generateMipmaps(texture.image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, texture.mipLevels);

	texture.imageView = createImageView(
		texture.image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		texture.mipLevels);
	texture.sampler = textureSampler;
}
void VulkanAndRTX::createTextureFromEmbedded(const std::string& embeddedTextureName, Texture& texture, const aiScene* scene) {
	const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(embeddedTextureName.c_str());

	if (!embeddedTexture) {
		throw std::runtime_error("Failed to find embedded texture: " + embeddedTextureName);
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	// Decode the embedded texture
	stbi_uc* pixels = nullptr;
	int texWidth, texHeight, texChannels;

	if (embeddedTexture->mHeight == 0) {
		// Compressed image data (e.g., PNG or JPEG)
		pixels = stbi_load_from_memory(reinterpret_cast<stbi_uc*>(embeddedTexture->pcData),
			static_cast<int>(embeddedTexture->mWidth), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	}
	else {
		// Raw pixel data
		texWidth = embeddedTexture->mWidth;
		texHeight = embeddedTexture->mHeight;
		pixels = reinterpret_cast<stbi_uc*>(embeddedTexture->pcData);
		texChannels = 4;  // Assuming RGBA
	}

	if (!pixels) {
		throw std::runtime_error("Failed to load embedded texture!");
	}

	VkDeviceSize imageSize = texWidth * texHeight * 4; // 4 bytes per pixel for RGBA
	uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(vkInit.device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(vkInit.device, stagingBufferMemory);

	if (embeddedTexture->mHeight == 0) {
		stbi_image_free(pixels);
	}

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	createImage(texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT,
		VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
	copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	vkDestroyBuffer(vkInit.device, stagingBuffer, nullptr);
	vkFreeMemory(vkInit.device, stagingBufferMemory, nullptr);

	generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);

	texture.image = textureImage;
	texture.imageMemory = textureImageMemory;
	texture.imageView = createImageView(
		texture.image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		texture.mipLevels);
	texture.sampler = textureSampler;

	std::cout << embeddedTextureName << " " << texWidth << " " << texHeight << "\n";
}
Texture VulkanAndRTX::LoadTexture(const std::string& texturePath, const aiScene* scene) {
	Texture texture{};

	// Embedded texture in .glb file, the path is in the form "*n", where n is the index
	if (texturePath[0] == '*') {
		createTextureFromEmbedded(texturePath, texture, scene);
	}
	else {
		createTextureImageFromPath(texturePath, texture);
	}

	return texture;
}
Material VulkanAndRTX::ProcessMaterial(aiMaterial* aiMat, const aiScene* scene) {
	Material material{};

	// Load diffuse texture
	if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString texturePath;
		aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
		material.diffuseTexture = LoadTexture(texturePath.C_Str(), scene);
	}

	return material;
}
static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene) {
	Mesh processedMesh;

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex{};
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mTextureCoords[0]) {
			vertex.texCoord0 = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else {
			vertex.texCoord0 = glm::vec2(0.0f, 0.0f);
		}
		processedMesh.vertices.push_back(vertex);
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++) {
			processedMesh.indices.push_back(face.mIndices[j]);
		}
	}

	return processedMesh;
}
void VulkanAndRTX::ProcessNode(aiNode* node, const aiScene* scene, ModelsBuffer* modelsBuffer, Model& parentModel) {
	for (size_t i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh processedMesh = ProcessMesh(mesh, scene);
		parentModel.meshes.push_back(processedMesh);

		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			Material processedMaterial = ProcessMaterial(material, scene);
			parentModel.materials.push_back(processedMaterial);
		}
	}
	for (size_t i = 0; i < node->mNumChildren; i++) {
		Model childModel;
		ProcessNode(node->mChildren[i], scene, modelsBuffer, childModel);
		modelsBuffer->models.push_back(childModel);
	}
}
void VulkanAndRTX::LoadModelsFromDirectory(const std::string& directory, ModelsBuffer* modelsBuffer) {
	auto modelFiles = GetModelFiles(directory);

	for (const auto& file : modelFiles) {
		std::cout << "Loading model: " << file << std::endl;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "Failed to load model " << file << ": " << importer.GetErrorString() << std::endl;
			continue;
		}

		Model rootModel;
		ProcessNode(scene->mRootNode, scene, modelsBuffer, rootModel);
		modelsBuffer->models.push_back(rootModel);
	}
}
