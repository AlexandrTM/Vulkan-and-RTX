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

	std::vector <Vertex> localVertices(24);
#pragma region
	localVertices[0].pos = { glm::vec3(0.0f    , 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[1].pos = { glm::vec3(cubeSize, 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[2].pos = { glm::vec3(cubeSize, cubeSize, 0.0f) + glm::vec3(x, y, z) };
	localVertices[3].pos = { glm::vec3(0.0f    , cubeSize, 0.0f) + glm::vec3(x, y, z) };
	localVertices[4].pos = { glm::vec3(0.0f    , 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[5].pos = { glm::vec3(cubeSize, 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[6].pos = { glm::vec3(cubeSize, cubeSize, cubeSize) + glm::vec3(x, y, z) };
	localVertices[7].pos = { glm::vec3(0.0f    , cubeSize, cubeSize) + glm::vec3(x, y, z) };

	localVertices[8].pos = { glm::vec3(0.0f    , 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[9].pos = { glm::vec3(cubeSize, 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[10].pos = { glm::vec3(cubeSize, cubeSize, 0.0f) + glm::vec3(x, y, z) };
	localVertices[11].pos = { glm::vec3(0.0f    , cubeSize, 0.0f) + glm::vec3(x, y, z) };
	localVertices[12].pos = { glm::vec3(0.0f    , 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[13].pos = { glm::vec3(cubeSize, 0.0f    , cubeSize) + glm::vec3(x, y, z) };
	localVertices[14].pos = { glm::vec3(cubeSize, cubeSize, cubeSize) + glm::vec3(x, y, z) };
	localVertices[15].pos = { glm::vec3(0.0f    , cubeSize, cubeSize) + glm::vec3(x, y, z) };

	localVertices[16].pos = { glm::vec3(0.0f    , 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[17].pos = { glm::vec3(cubeSize, 0.0f    , 0.0f) + glm::vec3(x, y, z) };
	localVertices[18].pos = { glm::vec3(cubeSize, cubeSize, 0.0f) + glm::vec3(x, y, z) };
	localVertices[19].pos = { glm::vec3(0.0f    , cubeSize, 0.0f) + glm::vec3(x, y, z) };
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

	uint32_t verticesNum = static_cast<uint32_t>(vertices.size());
	std::vector<uint32_t> localIndices = {
		0 + verticesNum, 3 + verticesNum, 1 + verticesNum,
		1 + verticesNum, 3 + verticesNum, 2 + verticesNum,
		4 + verticesNum, 5 + verticesNum, 7 + verticesNum,
		7 + verticesNum, 5 + verticesNum, 6 + verticesNum,

		11 + verticesNum, 15 + verticesNum, 10 + verticesNum,
		10 + verticesNum, 15 + verticesNum, 14 + verticesNum,
		12 + verticesNum, 8 + verticesNum, 13 + verticesNum,
		13 + verticesNum, 8 + verticesNum, 9 + verticesNum,

		20 + verticesNum, 23 + verticesNum, 16 + verticesNum,
		16 + verticesNum, 23 + verticesNum, 19 + verticesNum,
		17 + verticesNum, 18 + verticesNum, 21 + verticesNum,
		21 + verticesNum, 18 + verticesNum, 22 + verticesNum
	};

	// indices and vertices push back
	for (size_t i = 0; i < localVertices.size(); i++)
	{
		vertices.push_back(localVertices[i]);
	}
	for (size_t i = 0; i < localIndices.size(); i++)
	{
		indices.push_back(localIndices[i]);
	}
}

void VulkanAndRTX::loadModel(std::string modelPath)
{
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
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}
}

void VulkanAndRTX::loadGltfModel(const std::string& filePath) {
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string error;
	std::string warning;

	bool binary = false;
	size_t extPos = filePath.rfind('.', filePath.length());
	if (extPos != std::string::npos) {
		binary = (filePath.substr(extPos + 1, filePath.length() - extPos - 1) == "glb");
	}

	bool result = binary ? loader.LoadBinaryFromFile(&model, &error, &warning, filePath.c_str())
		: loader.LoadASCIIFromFile(&model, &error, &warning, filePath.c_str());

	if (!result) {
		std::cout << "model not loaded: " + filePath << "\n";
	}

	for (const auto& mesh : model.meshes) {
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
				const tinygltf::Accessor& positionAccessor = model.accessors[attributes.find("POSITION")->second];
				const tinygltf::BufferView& posView = model.bufferViews[positionAccessor.bufferView];
				bufferPositions = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[positionAccessor.byteOffset + posView.byteOffset]));

				// normals
				if (attributes.find("NORMAL") != attributes.end()) {
					const tinygltf::Accessor& normalAccessor = model.accessors[attributes.find("NORMAL")->second];
					const tinygltf::BufferView& normalView = model.bufferViews[normalAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normalView.buffer].data[normalAccessor.byteOffset + normalView.byteOffset]));
				}

				// texCoords
				if (attributes.find("TEXCOORD_0") != attributes.end()) {
					const tinygltf::Accessor& texCoordsAccessor0 = model.accessors[attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& texCoordsView0 = model.bufferViews[texCoordsAccessor0.bufferView];
					bufferTexCoordSet0 = reinterpret_cast<const float*>(&(model.buffers[texCoordsView0.buffer].data[texCoordsAccessor0.byteOffset + texCoordsView0.byteOffset]));
				}
				if (attributes.find("TEXCOORD_1") != attributes.end()) {
					const tinygltf::Accessor& texCoordsAccessor1 = model.accessors[attributes.find("TEXCOORD_1")->second];
					const tinygltf::BufferView& texCoordsView1 = model.bufferViews[texCoordsAccessor1.bufferView];
					bufferTexCoordSet1 = reinterpret_cast<const float*>(&(model.buffers[texCoordsView1.buffer].data[texCoordsAccessor1.byteOffset + texCoordsView1.byteOffset]));
				}

				// Extract INDICES
				{
					const tinygltf::Accessor& indicesAccessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
					const tinygltf::BufferView& indicesView = model.bufferViews[indicesAccessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[indicesView.buffer];

					const unsigned char* indicesPreData =
						&(buffer.data[indicesAccessor.byteOffset + indicesView.byteOffset]);

					switch (indicesAccessor.componentType)
					{
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
						const uint8_t* indicesData = reinterpret_cast<const uint8_t*>(indicesPreData);

						for (size_t i = 0; i < indicesAccessor.count; ++i) {
							indices.push_back(indicesData[i]);
						}
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
						const uint16_t* indicesData = reinterpret_cast<const uint16_t*>(indicesPreData);

						for (size_t i = 0; i < indicesAccessor.count; ++i) {
							indices.push_back(indicesData[i]);
						}
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
						const uint32_t* indicesData = reinterpret_cast<const uint32_t*>(indicesPreData);

						for (size_t i = 0; i < indicesAccessor.count; ++i) {
							indices.push_back(indicesData[i]);
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

					vertices.push_back(vertex);
				}
			}
		}
	}
	//std::cout << "vertices size: " << vertices.size() * sizeof(vertices) << "\n";
}
