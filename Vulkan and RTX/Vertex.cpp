#include "pch.h"
#include "Vertex.h"

// number of bytes between data entries and how move to the next data entry
VkVertexInputBindingDescription Vertex::getBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

// how to extract a vertex attribute from a chunk of vertex data originating from a binding description to shaders
std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions(VertexLayoutType vertexLayoutType)
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

	switch (vertexLayoutType) {
	case VertexLayoutType::POSITION_ONLY:
		attributeDescriptions.resize(1);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);
		break;
	case VertexLayoutType::POSITION_TEXCOORDS:
		attributeDescriptions.resize(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, texCoord0);
		break;
	case VertexLayoutType::WHOLE:
		attributeDescriptions.resize(7);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, normal);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, color);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, texCoord0);

		attributeDescriptions[4].binding = 0;
		attributeDescriptions[4].location = 4;
		attributeDescriptions[4].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[4].offset = offsetof(Vertex, texCoord1);

		attributeDescriptions[5].binding = 0;
		attributeDescriptions[5].location = 5;
		attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SINT;
		attributeDescriptions[5].offset = offsetof(Vertex, boneIDs);

		attributeDescriptions[6].binding = 0;
		attributeDescriptions[6].location = 6;
		attributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[6].offset = offsetof(Vertex, boneWeights);
		break;
	default:
		std::cout << "wrong vertexLayoutType\n";
		break;
	}

	return attributeDescriptions;
}

bool Vertex::operator==(const Vertex& other) const
{
	return 
		position	== other.position     && 
		normal		== other.normal       && 
		color       == other.color	      && 
		texCoord0   == other.texCoord0    && 
		texCoord1   == other.texCoord1    && 
		boneIDs     == other.boneIDs      && 
		boneWeights == other.boneWeights;
}
