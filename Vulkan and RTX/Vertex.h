#include "pch.h"

#ifndef VERTEX_H
#define VERTEX_H

struct Vertex {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 normal;
	alignas(16) glm::vec3 color = glm::vec3(1.0f);
	alignas(16) glm::vec2 texCoord0;
	alignas(16) glm::vec2 texCoord1;

	alignas(16) uint32_t boneIDs[4] = { 0 }; // IDs of up to 4 influencing bones
	alignas(16) float boneWeights[4] = { 0.0f }; // Corresponding weights of the bones

	// number of bytes between data entries and how move to the next data entry
	static VkVertexInputBindingDescription getBindingDescription();

	// how to extract a vertex attribute from a chunk of vertex data originating from a binding description to shaders
	static std::array<VkVertexInputAttributeDescription, 7> getAttributeDescriptions();

	bool operator==(const Vertex& other) const;
};

// hash function for user-defined type "vertex"
namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const
		{
			auto h1 = hash<glm::vec3>()(vertex.position);
			auto h2 = hash<glm::vec3>()(vertex.normal);
			auto h3 = hash<glm::vec3>()(vertex.color);
			auto h4 = hash<glm::vec2>()(vertex.texCoord0);
			auto h5 = hash<glm::vec2>()(vertex.texCoord1);

			return ((((((
				   h1 
				^ (h2 << 1)) >> 1)
				^ (h3 << 1)) >> 1)
				^ (h4 << 1)) >> 1)
				^ (h5 << 1);
		}
	};
}

#endif // !VERTEX_H
