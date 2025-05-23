#include "pch.h"
#include "ModelManager.h"
#include "AetherEngine.h"
#include "Vertex.h"

std::vector<Model> ModelManager::generateCubicLandscape(
	size_t landscapeWidth, size_t landscapeLenght, 
	float cubeSize,
	glm::vec3 color,
	Texture& texture,
	ModelType modelType
)
{
	std::vector<Model> models;
	for (size_t i = 0; i < landscapeWidth; i++)
	{
		for (size_t j = 0; j < landscapeLenght; j++)
		{
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
Model ModelManager::createCube(
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
Model ModelManager::createCuboid(
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
	for (size_t i = 0; i < localVertices.size(); i++)
	{
		mesh.vertices.push_back(localVertices[i]);
	}
	for (size_t i = 0; i < localIndices.size(); i++)
	{
		mesh.indices.push_back(localIndices[i]);
	}

	mesh.material = material;

	model.meshes.push_back(mesh);
	//model.isCollidable = true;
	return model;
}
Model ModelManager::createQuad(
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
	//material.diffuseTexture.uniqueHash = material.diffuseTexture.randomHash64();

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
void ModelManager::createSkyModel(Model& model)
{
	std::vector<Vertex> localVertices(24);
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
	for (size_t i = 0; i < localVertices.size(); i++)
	{
		mesh.vertices.push_back(localVertices[i]);
	}
	for (size_t i = 0; i < localIndices.size(); i++)
	{
		mesh.indices.push_back(localIndices[i]);
	}
	model.meshes.push_back(mesh);
}

void ModelManager::loadObjModel(const std::string& modelPath, std::vector<Model>& models)
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

			vertex.color = glm::vec3(1.0f);

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
	models.push_back(model);
}
void ModelManager::loadGltfModel(const std::string& modelPath, std::vector<Model>& models) {
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
		std::cout << "rootModel not loaded: " + modelPath << "\n";
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

					vertex.color = glm::vec3(1.0f);

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
	models.push_back(model);
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
static glm::mat4 assimpToGLMMat4(const aiMatrix4x4& from) {
	glm::mat4 to{};
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}
static void decomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::quat& rotation, glm::vec3& scale) {
	// Extract the translation
	position = glm::vec3(transform[3]);

	// Extract the scale
	scale = glm::vec3(
		glm::length(transform[0]),
		glm::length(transform[1]),
		glm::length(transform[2])
	);

	// Extract the rotation
	glm::mat3 rotationMatrix = glm::mat3(
		transform[0] / scale.x,
		transform[1] / scale.y,
		transform[2] / scale.z
	);
	rotation = glm::quat_cast(rotationMatrix);
}
static glm::mat4 setScaleToOne(const glm::mat4& matrix) {
	glm::vec3 position, scale;
	glm::quat rotation;

	decomposeTransform(matrix, position, rotation, scale);

	// Reconstruct the matrix with the updated scale
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);

	return translationMatrix * rotationMatrix * scaleMatrix;
}

// creating texture for MSAA sampling
void AetherEngine::createColorTexture(Texture& texture)
{
	if (texture.uniqueHash == 0) {
		texture.uniqueHash = randomHash64();
	}

	VkFormat colorFormat = swapchainImageFormat;
	uint32_t mipLevels = 1;

	createImage(
		swapchainExtent.width, swapchainExtent.height, mipLevels, vkInit.colorSamples, colorFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texture.image, texture.vmaAllocation
	);

	texture.imageView = createImageView(texture.image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
}
void AetherEngine::createDepthTexture(Texture& texture)
{
	if (texture.uniqueHash == 0) {
		texture.uniqueHash = randomHash64();
	}

	VkFormat depthFormat = findDepthFormat();
	uint32_t mipLevels = 1;

	createImage(
		swapchainExtent.width, swapchainExtent.height, mipLevels, vkInit.colorSamples, depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texture.image, texture.vmaAllocation
	);

	texture.imageView = createImageView(texture.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, mipLevels);

	transitionImageLayout(
		texture.image, depthFormat,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		mipLevels
	);
}

void AetherEngine::uploadRawDataToTexture(void* rawImage, uint32_t width, uint32_t height, Texture& texture)
{
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	VkDeviceSize imageSize = sizeof(uint8_t) * 4 * width * height;

	createBuffer(
		imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true,
		stagingBuffer, stagingAllocation
	);

	void* data;
	vmaMapMemory(vmaAllocator, stagingAllocation, &data);
	memcpy(data, rawImage, static_cast<size_t>(imageSize));
	vmaUnmapMemory(vmaAllocator, stagingAllocation);

	transitionImageLayout(
		texture.image, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		texture.mipLevels
	);

	copyBufferToImage(stagingBuffer, texture.image, width, height);

	transitionImageLayout(
		texture.image, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		texture.mipLevels
	);

	vmaDestroyBuffer(vmaAllocator, stagingBuffer, stagingAllocation);
	destroyedVmaAllocations += 1;
}
void AetherEngine::createSolidColorTexture(
	std::array<uint8_t, 4> color, uint32_t width, uint32_t height, Texture& texture
)
{
	if (texture.uniqueHash == 0) {
		texture.uniqueHash = randomHash64();
	}

	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;

	VkDeviceSize imageSize = sizeof(uint8_t) * 4 * width * height;
	texture.mipLevels = 1;
	texture.width = width;
	texture.height = height;

	createBuffer(
		imageSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true,
		stagingBuffer, stagingAllocation
	);
	
	// filing whole buffer with the same color
	std::vector<uint8_t> pixelData(imageSize);
	for (size_t i = 0; i < static_cast<size_t>(width * height); ++i) {
		std::memcpy(&pixelData[i * 4], color.data(), 4);
	}

	void* data;
	vmaMapMemory(vmaAllocator, stagingAllocation, &data);
	memcpy(data, pixelData.data(), static_cast<size_t>(imageSize));
	vmaUnmapMemory(vmaAllocator, stagingAllocation);

	createImage(
		width, height, texture.mipLevels, VK_SAMPLE_COUNT_1_BIT,
		VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texture.image, texture.vmaAllocation
	);

	transitionImageLayout(
		texture.image, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		texture.mipLevels
	);

	copyBufferToImage(
		stagingBuffer, texture.image,
		width, height
	);

	vmaDestroyBuffer(vmaAllocator, stagingBuffer, stagingAllocation);
	destroyedVmaAllocations += 1;

	transitionImageLayout(
		texture.image, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		texture.mipLevels
	);

	texture.imageView = createImageView(
		texture.image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		texture.mipLevels);
	createTextureSampler(texture.mipLevels, texture.sampler);
}
Texture AetherEngine::loadTextureFromPath(const std::string& texturePath)
{
	Texture texture;

	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	
	if (!pixels) {
		std::cout << texturePath;
		throw std::runtime_error("failed to load texture image!");
	}

	VkDeviceSize imageSize = texWidth * texHeight * 4;
	texture.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
	texture.width = texWidth;
	texture.height = texHeight;

	createBuffer(
		imageSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true,
		stagingBuffer, stagingAllocation
	);

	void* data;
	vmaMapMemory(vmaAllocator, stagingAllocation, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vmaUnmapMemory(vmaAllocator, stagingAllocation);

	stbi_image_free(pixels);

	createImage(
		texWidth, texHeight, texture.mipLevels, VK_SAMPLE_COUNT_1_BIT, 
		VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, 
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texture.image, texture.vmaAllocation
	);

	transitionImageLayout(
		texture.image, VK_FORMAT_R8G8B8A8_SRGB, 
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
		texture.mipLevels
	);

	copyBufferToImage(
		stagingBuffer, texture.image,
		static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)
	);

	vmaDestroyBuffer(vmaAllocator, stagingBuffer, stagingAllocation);
	destroyedVmaAllocations += 1;

	generateMipmaps(texture.image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, texture.mipLevels);

	texture.imageView = createImageView(
		texture.image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		texture.mipLevels
	);
	createTextureSampler(texture.mipLevels, texture.sampler);

	return texture;
}
void AetherEngine::createTextureFromEmbedded(
	const std::string& embeddedTextureName,
	const aiScene* scene, Texture& texture
) {
	if (texture.uniqueHash == 0) {
		texture.uniqueHash = randomHash64();
	}

	const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(embeddedTextureName.c_str());

	if (!embeddedTexture) {
		throw std::runtime_error("Failed to find embedded texture: " + embeddedTextureName);
	}

	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;

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
	texture.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
	texture.width = texWidth;
	texture.height = texHeight;

	createBuffer(
		imageSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true,
		stagingBuffer, stagingAllocation
	);

	void* data;
	vmaMapMemory(vmaAllocator, stagingAllocation, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vmaUnmapMemory(vmaAllocator, stagingAllocation);

	if (embeddedTexture->mHeight == 0) {
		stbi_image_free(pixels);
	}

	createImage(
		texWidth, texHeight, texture.mipLevels, VK_SAMPLE_COUNT_1_BIT,
		VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		texture.image, texture.vmaAllocation
	);

	transitionImageLayout(
		texture.image, VK_FORMAT_R8G8B8A8_SRGB, 
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
		texture.mipLevels
	);
	copyBufferToImage(
		stagingBuffer, texture.image, 
		static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)
	);

	vmaDestroyBuffer(vmaAllocator, stagingBuffer, stagingAllocation);
	destroyedVmaAllocations += 1;

	generateMipmaps(texture.image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, texture.mipLevels);

	texture.imageView = createImageView(
		texture.image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		texture.mipLevels
	);
	createTextureSampler(texture.mipLevels, texture.sampler);

	/*std::cout << "Embedded Texture Loaded:\n";
	std::cout << " - Name: " << embeddedTextureName << "\n";
	std::cout << " - Dimensions: " << width << "x" << height << "\n";
	std::cout << " - Channels: " << texChannels << "\n";
	std::cout << " - Image Size: " << imageSize / (1024.0f * 1024.0f) << " MB\n";
	std::cout << " - Mip Levels: " << texture.mipLevels << "\n";
	std::cout << " - Compression: " << (embeddedTexture->mHeight == 0 ? "Yes" : "No") << "\n";*/
}
Texture AetherEngine::loadTextureForModel(const std::string& texturePath, const aiScene* scene) 
{
	Texture texture;

	// Embedded texture in .glb file, the path is in the form "*n", where n is the index
	if (texturePath[0] == '*') {
		createTextureFromEmbedded(texturePath, scene, texture);
	}
	else {
		texture = loadTextureFromPath(("textures\\" + std::string(texturePath)));
	}

	return texture;
}
void AetherEngine::loadTexturesFromFolder(
	const std::string& texturePath,
	std::unordered_map<std::string, Texture>& textures
)
{
	for (const auto& entry : std::filesystem::directory_iterator(texturePath)) {
		if (!entry.is_regular_file()) continue;
		//std::cout << "entry: " << entry << "\n";
		std::string path = entry.path().string();
		std::string name = entry.path().stem().string();

		textures[name] = loadTextureFromPath(path);
	}
}

Material AetherEngine::processMaterial(aiMaterial* aiMat, const aiScene* scene) 
{
	Material material{};

	//std::cout << "Material Name: " << aiMat->GetName().C_Str() << "\n";
	//// Print all texture types
	//for (int textureType = aiTextureType_NONE; textureType <= aiTextureType_MAYA_SPECULAR_ROUGHNESS; ++textureType) {
	//	int textureCount = aiMat->GetTextureCount(static_cast<aiTextureType>(textureType));
	//	if (textureCount > 0) {
	//		std::cout << "Texture Type " << textureType << ": " << textureCount << "\n";
	//	}
	//}

	if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		
		aiString texturePath;
		if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
			material.diffuseTexture = std::move(loadTextureForModel(texturePath.C_Str(), scene));
		}
	}

	if (aiMat->GetTextureCount(aiTextureType_EMISSIVE) > 0) {

		aiString texturePath;
		if (aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath) == AI_SUCCESS) {
			material.emissiveTexture = std::move(loadTextureForModel(texturePath.C_Str(), scene));
		}
	}

	return material;
}
static void processAnimations(const aiScene* scene, Model& model) {
	if (scene->HasAnimations()) {
		std::cout << "scene: " << scene->mName.C_Str() <<
			" has animations: " << scene->mNumAnimations << "\n";
	}

	for (size_t i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation* animation = scene->mAnimations[i];

		Animation processedAnimation;
		processedAnimation.name = animation->mName.C_Str();
		processedAnimation.duration = animation->mDuration;
		processedAnimation.ticksPerSecond = animation->mTicksPerSecond != 0.0 ? animation->mTicksPerSecond : 25.0;

		// Process animation channels
		for (size_t j = 0; j < animation->mNumChannels; j++) {
			aiNodeAnim* channel = animation->mChannels[j];

			AnimationChannel processedChannel;
			processedChannel.nodeName = channel->mNodeName.C_Str();

			// Load position keys
			for (size_t k = 0; k < channel->mNumPositionKeys; k++) {
				aiVectorKey positionKey = channel->mPositionKeys[k];
				processedChannel.positionKeys.emplace_back(
					positionKey.mTime,
					glm::vec3(positionKey.mValue.x, positionKey.mValue.y, positionKey.mValue.z)
				);
			}

			// Load rotation keys
			for (size_t k = 0; k < channel->mNumRotationKeys; k++) {
				aiQuatKey rotationKey = channel->mRotationKeys[k];
				processedChannel.rotationKeys.emplace_back(
					rotationKey.mTime,
					glm::quat(rotationKey.mValue.w, rotationKey.mValue.x, rotationKey.mValue.y, rotationKey.mValue.z)
				);
			}

			// Load scaling keys
			for (size_t k = 0; k < channel->mNumScalingKeys; k++) {
				aiVectorKey scalingKey = channel->mScalingKeys[k];
				processedChannel.scalingKeys.emplace_back(
					scalingKey.mTime,
					glm::vec3(scalingKey.mValue.x, scalingKey.mValue.y, scalingKey.mValue.z)
				);
			}

			processedAnimation.channels.push_back(processedChannel);
		}

		model.animations.push_back(processedAnimation);
	}
}

static aiNode* findNode(aiNode* rootNode, const std::string& name) {
	if (name == rootNode->mName.C_Str()) {
		return rootNode;
	}
	for (size_t i = 0; i < rootNode->mNumChildren; ++i) {
		aiNode* foundNode = findNode(rootNode->mChildren[i], name);
		if (foundNode) {
			return foundNode;
		}
	}
	return nullptr;
}
static void processBones(aiMesh* mesh, const aiScene* scene, Mesh& processedMesh)
{
	std::vector<uint32_t> boneCount(mesh->mNumVertices, 0);
	for (size_t i = 0; i < mesh->mNumBones; i++) {
		aiBone* bone = mesh->mBones[i];
		std::string boneName = bone->mName.C_Str();

		size_t boneIndex;
		if (processedMesh.boneMap.find(boneName) == processedMesh.boneMap.end()) {
			boneIndex = processedMesh.bones.size();
			Bone processedBone{};
			processedBone.name = boneName;
			//processedBone.offsetMatrix = assimpToGLMMat4(bone->mOffsetMatrix);
			//processedBone.globalTransform = globalTransform;
			processedBone.finalTransform = processedBone.offsetMatrix;

			processedMesh.bones.push_back(processedBone);
			processedMesh.boneMap[boneName] = boneIndex;

			/*glm::vec3 position, scale;
			glm::quat rotation;
			decomposeTransform(processedBone.offsetMatrix, position, rotation, scale);
			std::cout << "boneName: " << boneName << "\n";
			std::cout << "Position: " << glm::to_string(position) << "\n";
			std::cout << "Rotation: " << glm::to_string(rotation) << "\n";
			std::cout << "Scale: " << glm::to_string(scale) << "\n";*/
		}
		else {
			boneIndex = processedMesh.boneMap[boneName];
		}

		// Assign bone weights to vertices
		for (size_t j = 0; j < bone->mNumWeights; j++) {
			uint32_t vertexID = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;

			if (boneCount[vertexID] < 4) {
				processedMesh.vertices[vertexID].boneIDs[boneCount[vertexID]] = boneIndex;
				processedMesh.vertices[vertexID].boneWeights[boneCount[vertexID]] = weight;
				boneCount[vertexID]++;
			}
		}
	}

	// Normalize weights for vertices
	for (auto& vertex : processedMesh.vertices) {
		float totalWeight =
			vertex.boneWeights[0] + vertex.boneWeights[1] +
			vertex.boneWeights[2] + vertex.boneWeights[3];

		if (totalWeight > 0.0f) {
			for (size_t i = 0; i < 4; i++) {
				vertex.boneWeights[i] /= totalWeight;
			}
		}
	}
}

static Mesh processMesh(
	aiMesh* mesh,
	Model& parentModel,
	glm::mat4 globalTransform,
	glm::mat4 globalInverseTransform,
	uint32_t& perModelVertexOffset,
	const aiScene* scene
) {
	Mesh processedMesh;

	// position, normal, tex coords, color
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
		//vertex.color = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mColors[0]) {
			vertex.color = glm::vec3(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b);
		}
		else {
			vertex.color = glm::vec3(1.0f);
		}

		processedMesh.vertices.push_back(vertex);
	}

	// indices
	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++) {
			processedMesh.indices.push_back(face.mIndices[j]);
		}
	}

	// bones
	processBones(mesh, scene, processedMesh);

	perModelVertexOffset += mesh->mNumVertices;

	return processedMesh;
}

void AetherEngine::processNode(
	aiNode* node, const aiScene* scene, 
	std::vector<Model>& models, 
	Model& parentModel, 
	glm::mat4 parentTransform, const glm::mat4 globalInverseTransform,
	uint32_t& perModelVertexOffset,
	int level
) {
	glm::mat4 nodeTransform = assimpToGLMMat4(node->mTransformation);
	glm::mat4 globalTransform = parentTransform * nodeTransform;
	//globalTransform = setScaleToOne(globalTransform);

	// node hierarchy
	/*glm::vec3 position, scale;
	glm::quat rotation;
	decomposeTransform(globalTransform, position, rotation, scale);

	std::cout << std::string(level * 2, ' ') << "Node: " << node->mName.C_Str() << "\n";
	std::cout << std::string(level * 2, ' ') << "  Position: " << glm::to_string(position) << "\n";
	std::cout << std::string(level * 2, ' ') << "  Rotation: " << glm::to_string(rotation) << "\n";
	std::cout << std::string(level * 2, ' ') << "  Scale: " << glm::to_string(scale) << "\n";*/

	for (size_t i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh processedMesh = processMesh(
			mesh, parentModel,
			globalTransform,
			globalInverseTransform,
			perModelVertexOffset,
			scene
		);
		processedMesh.transform = globalTransform;

		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			Material processedMaterial = processMaterial(material, scene);
			processedMesh.material = processedMaterial;
		}

		parentModel.meshes.push_back(processedMesh);
	}

	for (size_t i = 0; i < node->mNumChildren; i++) {
		Model childModel;
		processNode(
			node->mChildren[i], scene, models, 
			childModel, globalTransform, 
			globalInverseTransform, perModelVertexOffset, 
			level + 1
		);
		//if (childModel.meshes.size() > 0) {
			models.push_back(childModel);
		//}
	}
}
void AetherEngine::loadModelsFromFolder(
	const std::string& directory, 
	std::vector<Model>& models
) {
	auto modelFiles = GetModelFiles(directory);
	
	for (const auto& file : modelFiles) {
		std::cout << "Loading rootModel: " << file << std::endl;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "Failed to load rootModel " << file << ": " << importer.GetErrorString() << std::endl;
			continue;
		}

		Model rootModel;
		uint32_t perModelVertexOffset = 0;
		glm::mat4 identityTransform = glm::mat4(1.0f);
		glm::mat4 globalInverseTransform = glm::inverse(assimpToGLMMat4(scene->mRootNode->mTransformation));

		/*size_t meshesNum = 0;
		for (size_t i = 0; i < models.size(); i++) {
			meshesNum += models[i].meshes.size();
		}
		std::cout << "models.size(): " << models.size() << "\n";
		std::cout << "meshes num: " << meshesNum << "\n";*/

		processNode(
			scene->mRootNode, scene, models, rootModel, 
			identityTransform, globalInverseTransform, 
			perModelVertexOffset, 0
		);
		processAnimations(scene, rootModel);
		models.push_back(rootModel);

		/*meshesNum = 0;
		for (size_t i = 0; i < models.size(); i++) {
			meshesNum += models[i].meshes.size();
		}
		std::cout << "models.size(): " << models.size() << "\n";
		std::cout << "meshes num: " << meshesNum << "\n";*/
	}
}
