#include "pch.h"

#ifndef VERTEX_H
#define VERTEX_H

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texCoord;

	// number of bytes between data entries and how move to the next data entry
	static VkVertexInputBindingDescription getBindingDescription();

	// how to extract a vertex attribute from a chunk of vertex data originating from a binding description to shaders
	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();

	bool operator==(const Vertex& other) const;
};

// hash function for user-defined type "vertex"
namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const
		{
			auto h1 = hash<glm::vec3>()(vertex.pos);
			auto h2 = hash<glm::vec3>()(vertex.color);
			auto h3 = hash<glm::vec2>()(vertex.texCoord);
			auto h4 = hash<glm::vec3>()(vertex.normal);

			return ((((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1)) >> 1) ^ (h4 << 1);
		}
	};
}

#endif // !VERTEX_H
