#include "pch.h"
#include "Vertex.h"

#ifndef MODEL
#define MODEL

struct Model
{
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
    VkBuffer              vertexBuffer;
    VkDeviceMemory        vertexBufferMemory;
    VkBuffer              indexBuffer;
    VkDeviceMemory        indexBufferMemory;
};

#endif // !MODEL