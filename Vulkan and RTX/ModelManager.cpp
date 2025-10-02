#include "pch.h"
#include "AetherEngine.h"
#include "Vertex.h"

ModelManager::ModelManager(
	VulkanInitializer& vkInitRef, 
	BufferManager& bufferManagerRef,
	ImageManager& imageManagerRef
)
	: vkInit(vkInitRef), bufferManager(bufferManagerRef), imageManager(imageManagerRef) {}

std::vector<std::string> ModelManager::GetModelFiles(const std::string& directory) {
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
glm::mat4 ModelManager::assimpToGLMMat4(const aiMatrix4x4& from) {
	glm::mat4 to{};
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}
void ModelManager::decomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::quat& rotation, glm::vec3& scale) {
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
glm::mat4 ModelManager::setScaleToOne(const glm::mat4& matrix) {
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
void ModelManager::createColorTexture(VkFormat& imageFormat, uint32_t width, uint32_t height, Texture& texture)
{
	if (texture.hash == 0) {
		texture.hash = randomHash64();
	}

	VkFormat colorFormat = imageFormat;
	uint32_t mipLevels = 1;

	imageManager.createImage(
		width, height, mipLevels, vkInit.colorSamples, colorFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texture.image, texture.vmaAllocation
	);

	texture.imageView = imageManager.createImageView(
		vkInit,
		texture.image, 
		colorFormat, 
		VK_IMAGE_ASPECT_COLOR_BIT, 
		mipLevels
	);
}
void ModelManager::createDepthTexture(uint32_t width, uint32_t height, Texture& texture)
{
	if (texture.hash == 0) {
		texture.hash = randomHash64();
	}

	VkFormat depthFormat = imageManager.findDepthFormat();
	uint32_t mipLevels = 1;

	imageManager.createImage(
		width, height, mipLevels, vkInit.colorSamples, depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texture.image, texture.vmaAllocation
	);

	texture.imageView = imageManager.createImageView(
		vkInit,
		texture.image, 
		depthFormat, 
		VK_IMAGE_ASPECT_DEPTH_BIT, 
		mipLevels
	);

	imageManager.transitionImageLayout(
		texture.image, depthFormat,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		mipLevels
	);
}
void ModelManager::uploadRawDataToTexture(void* rawImage, uint32_t width, uint32_t height, Texture& texture)
{
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	VkDeviceSize imageSize = sizeof(uint8_t) * 4 * width * height;

	bufferManager.createBuffer(
		imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true,
		stagingBuffer, stagingAllocation
	);

	void* data;
	vmaMapMemory(vkInit.vmaAllocator, stagingAllocation, &data);
	memcpy(data, rawImage, static_cast<size_t>(imageSize));
	vmaUnmapMemory(vkInit.vmaAllocator, stagingAllocation);

	imageManager.transitionImageLayout(
		texture.image, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		texture.mipLevels
	);

	imageManager.copyBufferToImage(stagingBuffer, texture.image, width, height);

	imageManager.transitionImageLayout(
		texture.image, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		texture.mipLevels
	);

	vmaDestroyBuffer(vkInit.vmaAllocator, stagingBuffer, stagingAllocation);
	gameContext.destroyedVmaAllocations += 1;
}

void ModelManager::createTextureFromPixelData(
	const void* pixelData,
	uint32_t width,
	uint32_t height,
	uint32_t mipLevels,
	Texture& texture
) {
	if (texture.hash == 0) {
		texture.hash = randomHash64();
	}

	texture.mipLevels = mipLevels;
	texture.width = width;
	texture.height = height;
	VkDeviceSize imageSize = sizeof(uint8_t) * 4 * width * height;

	// Create staging buffer
	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	bufferManager.createBuffer(
		imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true,
		stagingBuffer, stagingAllocation
	);

	// Copy pixels to staging buffer
	void* data;
	vmaMapMemory(vkInit.vmaAllocator, stagingAllocation, &data);
	memcpy(data, pixelData, static_cast<size_t>(imageSize));
	vmaUnmapMemory(vkInit.vmaAllocator, stagingAllocation);

	imageManager.createImage(
		width, height, mipLevels, VK_SAMPLE_COUNT_1_BIT,
		VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texture.image, texture.vmaAllocation
	);

	imageManager.transitionImageLayout(
		texture.image, VK_FORMAT_R8G8B8A8_SRGB, 
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED, 
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
		mipLevels
	);

	imageManager.copyBufferToImage(stagingBuffer, texture.image, width, height);

	vmaDestroyBuffer(vkInit.vmaAllocator, stagingBuffer, stagingAllocation);
	gameContext.destroyedVmaAllocations += 1;

	if (mipLevels > 1) {
		imageManager.generateMipmaps(texture.image, VK_FORMAT_R8G8B8A8_SRGB, width, height, mipLevels);
	}
	else {
		imageManager.transitionImageLayout(
			texture.image, VK_FORMAT_R8G8B8A8_SRGB, 
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
			mipLevels
		);
	}

	// Create view + sampler
	texture.imageView = imageManager.createImageView(
		vkInit,
		texture.image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT,
		mipLevels
	);
	imageManager.createTextureSampler(texture.mipLevels, texture.sampler);
}
void ModelManager::createSolidColorTexture(
	std::array<uint8_t, 4> color, uint32_t width, uint32_t height, Texture& texture
)
{
	VkDeviceSize imageSize = sizeof(uint8_t) * 4 * width * height;
	std::vector<uint8_t> pixelData(imageSize);
	for (size_t i = 0; i < static_cast<size_t>(width * height); ++i) {
		std::memcpy(&pixelData[i * 4], color.data(), 4);
	}
	createTextureFromPixelData(pixelData.data(), width, height, 1, texture);
}
Texture ModelManager::loadTextureFromPath(const std::string& texturePath)
{
	Texture texture;
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	
	if (!pixels) throw std::runtime_error("Failed to load texture: " + texturePath);

	uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	createTextureFromPixelData(pixels, texWidth, texHeight, mipLevels, texture);
	stbi_image_free(pixels);

	return texture;
}
void ModelManager::createTextureFromEmbedded(
	const std::string& embeddedTextureName,
	const aiScene* scene, Texture& texture
) {
	const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(embeddedTextureName.c_str());
	if (!embeddedTexture) throw std::runtime_error("Embedded texture not found: " + embeddedTextureName);

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

	if (!pixels) throw std::runtime_error("Failed to decode embedded texture: " + embeddedTextureName);
	
	uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	createTextureFromPixelData(pixels, texWidth, texHeight, mipLevels, texture);

	if (embeddedTexture->mHeight == 0) stbi_image_free(pixels);

	/*std::cout << "Embedded Texture Loaded:\n";
	std::cout << " - Name: " << embeddedTextureName << "\n";
	std::cout << " - Dimensions: " << width << "x" << height << "\n";
	std::cout << " - Channels: " << texChannels << "\n";
	std::cout << " - Image Size: " << imageSize / (1024.0f * 1024.0f) << " MB\n";
	std::cout << " - Mip Levels: " << texture.mipLevels << "\n";
	std::cout << " - Compression: " << (embeddedTexture->mHeight == 0 ? "Yes" : "No") << "\n";*/
}

Texture ModelManager::loadTextureForModel(const std::string& texturePath, const aiScene* scene) 
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
void ModelManager::loadTexturesFromFolder(
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

Material ModelManager::processMaterial(aiMaterial* aiMat, const aiScene* scene) 
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
void ModelManager::processAnimations(const aiScene* scene, Model& model) {
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

aiNode* ModelManager::findNode(aiNode* rootNode, const std::string& name) {
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
void ModelManager::processBones(aiMesh* mesh, const aiScene* scene, Mesh& processedMesh)
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

Mesh ModelManager::processMesh(
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

void ModelManager::processNode(
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
void ModelManager::loadModelsFromFolder(
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
