#include "pch.h"
#include "VulkanAndRTX.h"

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

void VulkanAndRTX::generateCube(float x, float y, float z, float_t cubeSize)
{
	glm::vec3 basicColor = glm::vec3(0.5f, 0.5f, 0.5f);

	std::vector<Vertex> localVertices(24);
	Model model;

#pragma region
	localVertices[0].pos  = { glm::vec3(0.0f    , 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[1].pos  = { glm::vec3(cubeSize, 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[2].pos  = { glm::vec3(cubeSize, cubeSize, 0.0f)     + glm::vec3(x, y, z) };
	localVertices[3].pos  = { glm::vec3(0.0f    , cubeSize, 0.0f)     + glm::vec3(x, y, z) };
	localVertices[4].pos  = { glm::vec3(0.0f    , 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[5].pos  = { glm::vec3(cubeSize, 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[6].pos  = { glm::vec3(cubeSize, cubeSize, cubeSize) + glm::vec3(x, y, z) };
	localVertices[7].pos  = { glm::vec3(0.0f    , cubeSize, cubeSize) + glm::vec3(x, y, z) };
	
	localVertices[8].pos  = { glm::vec3(0.0f    , 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[9].pos  = { glm::vec3(cubeSize, 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[10].pos = { glm::vec3(cubeSize, cubeSize, 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[11].pos = { glm::vec3(0.0f    , cubeSize, 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[12].pos = { glm::vec3(0.0f    , 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[13].pos = { glm::vec3(cubeSize, 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[14].pos = { glm::vec3(cubeSize, cubeSize, cubeSize) + glm::vec3(x, y, z) };
	localVertices[15].pos = { glm::vec3(0.0f    , cubeSize, cubeSize) + glm::vec3(x, y, z) };

	localVertices[16].pos = { glm::vec3(0.0f    , 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[17].pos = { glm::vec3(cubeSize, 0.0f    , 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[18].pos = { glm::vec3(cubeSize, cubeSize, 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[19].pos = { glm::vec3(0.0f    , cubeSize, 0.0f)	  + glm::vec3(x, y, z) };
	localVertices[20].pos = { glm::vec3(0.0f    , 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[21].pos = { glm::vec3(cubeSize, 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[22].pos = { glm::vec3(cubeSize, cubeSize, cubeSize) + glm::vec3(x, y, z) };
	localVertices[23].pos = { glm::vec3(0.0f    , cubeSize, cubeSize) + glm::vec3(x, y, z) };
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
		model.vertices.push_back(localVertices[i]);
	}
	for (size_t i = 0; i < localIndices.size(); i++)
	{
		model.indices.push_back(localIndices[i]);
	}
	models.objects.push_back(model);
}

void VulkanAndRTX::generateSkyCube()
{
	glm::vec3 basicColor = glm::vec3(0.5f, 0.5f, 0.5f);

	std::vector<Vertex> localVertices(24);
	Model model;

#pragma region
	localVertices[0].pos  = { glm::vec3(-1.0f, -1.0f, -1.0f) };
	localVertices[1].pos  = { glm::vec3( 1.0f, -1.0f, -1.0f) };
	localVertices[2].pos  = { glm::vec3( 1.0f,  1.0f, -1.0f) };
	localVertices[3].pos  = { glm::vec3(-1.0f,  1.0f, -1.0f) };
	localVertices[4].pos  = { glm::vec3(-1.0f, -1.0f,  1.0f) };
	localVertices[5].pos  = { glm::vec3( 1.0f, -1.0f,  1.0f) };
	localVertices[6].pos  = { glm::vec3( 1.0f,  1.0f,  1.0f) };
	localVertices[7].pos  = { glm::vec3(-1.0f,  1.0f,  1.0f) };

	localVertices[8].pos  = { glm::vec3(-1.0f, -1.0f, -1.0f) };
	localVertices[9].pos  = { glm::vec3( 1.0f, -1.0f, -1.0f) };
	localVertices[10].pos = { glm::vec3( 1.0f,  1.0f, -1.0f) };
	localVertices[11].pos = { glm::vec3(-1.0f,  1.0f, -1.0f) };
	localVertices[12].pos = { glm::vec3(-1.0f, -1.0f,  1.0f) };
	localVertices[13].pos = { glm::vec3( 1.0f, -1.0f,  1.0f) };
	localVertices[14].pos = { glm::vec3( 1.0f,  1.0f,  1.0f) };
	localVertices[15].pos = { glm::vec3(-1.0f,  1.0f,  1.0f) };

	localVertices[16].pos = { glm::vec3(-1.0f, -1.0f, -1.0f) };
	localVertices[17].pos = { glm::vec3( 1.0f, -1.0f, -1.0f) };
	localVertices[18].pos = { glm::vec3( 1.0f,  1.0f, -1.0f) };
	localVertices[19].pos = { glm::vec3(-1.0f,  1.0f, -1.0f) };
	localVertices[20].pos = { glm::vec3(-1.0f, -1.0f,  1.0f) };
	localVertices[21].pos = { glm::vec3( 1.0f, -1.0f,  1.0f) };
	localVertices[22].pos = { glm::vec3( 1.0f,  1.0f,  1.0f) };
	localVertices[23].pos = { glm::vec3(-1.0f,  1.0f,  1.0f) };
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
		model.vertices.push_back(localVertices[i]);
	}
	for (size_t i = 0; i < localIndices.size(); i++)
	{
		model.indices.push_back(localIndices[i]);
	}
	models.sky = model;
}

void VulkanAndRTX::generateTerrain(float startX, float startZ, size_t width, size_t height,
	float scale, float roughness, size_t seed)
{
	Model model;
	terrainGenerator = std::make_unique<TerrainGenerator>(seed);
	auto heightmap = terrainGenerator.get()->generateHeightMap(width, height, roughness);
	terrainGenerator.get()->generateTerrainMesh(startX, startZ, heightmap, scale, model);
	models.objects.push_back(model);
}

void VulkanAndRTX::loadModel(const std::string& modelPath)
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
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.pos = {
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
				uniqueVertices[vertex] = static_cast<uint32_t>(model.vertices.size());
				model.vertices.push_back(vertex);
			}

			model.indices.push_back(uniqueVertices[vertex]);
		}
		models.objects.push_back(model);
	}
}

void VulkanAndRTX::loadGltfModel(const std::string& modelPath) {
	tinygltf::Model GLTFmodel;
	Model model;

	tinygltf::TinyGLTF loader;
	std::string error;
	std::string warning;

	bool binary = false;
	size_t extPos = modelPath.rfind('.', modelPath.length());
	if (extPos != std::string::npos) {
		binary = (modelPath.substr(extPos + 1, modelPath.length() - extPos - 1) == "glb");
	}

	bool result = binary ? loader.LoadBinaryFromFile(&GLTFmodel, &error, &warning, modelPath.c_str())
		: loader.LoadASCIIFromFile(&GLTFmodel, &error, &warning, modelPath.c_str());

	if (!result) {
		std::cout << "model not loaded: " + modelPath << "\n";
	}

	for (const auto& mesh : GLTFmodel.meshes) {
		for (const auto& primitive : mesh.primitives) {

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

					switch (indicesAccessor.componentType)
					{
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
						const uint8_t* indicesData = reinterpret_cast<const uint8_t*>(indicesPreData);

						for (size_t i = 0; i < indicesAccessor.count; ++i) {
							model.indices.push_back(indicesData[i]);
						}
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
						const uint16_t* indicesData = reinterpret_cast<const uint16_t*>(indicesPreData);

						for (size_t i = 0; i < indicesAccessor.count; ++i) {
							model.indices.push_back(indicesData[i]);
						}
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
						const uint32_t* indicesData = reinterpret_cast<const uint32_t*>(indicesPreData);

						for (size_t i = 0; i < indicesAccessor.count; ++i) {
							model.indices.push_back(indicesData[i]);
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
					vertex.pos = glm::vec3(
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

					model.vertices.push_back(vertex);
				}
			}
		}
		models.objects.push_back(model);
	}
	//std::cout << "vertices size: " << vertices.size() * sizeof(vertices) << "\n";
}
