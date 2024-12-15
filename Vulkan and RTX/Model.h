#include "pch.h"
#include "Vertex.h"

#ifndef MODEL
#define MODEL

struct Texture {
    VkImage        image;
    VkDeviceMemory imageMemory;
    VkImageView    imageView;
    VkSampler      sampler;
    uint32_t       mipLevels;
};

struct Mesh
{
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
    VkBuffer              vertexBuffer;
    VkDeviceMemory        vertexBufferMemory;
    VkBuffer              indexBuffer;
    VkDeviceMemory        indexBufferMemory;
};

struct Material {
    Texture diffuseTexture;    // Diffuse map
    Texture normalTexture;     // Normal map (optional)
    Texture specularTexture;   // Specular map (optional)
    // Add more texture maps if needed
};

struct Model {
    std::vector<Mesh>     meshes;        // Separate meshes for complex models
    std::vector<Material> materials;     // Materials for different parts of the model
    glm::vec3             position;      // Position in the world
    glm::vec3             scale;         // Scale for the model
    glm::quat             rotation;      // Orientation of the model

    // Optional: Add bounding box for frustum culling
    glm::vec3             minBounds;
    glm::vec3             maxBounds;

    // LOD levels
    std::vector<std::vector<Mesh>> lodLevels;

    bool                  isLoaded = false;  // Dynamic loading flag
};

struct ModelsBuffer {
    std::vector<Model> models;
    Model sky;
};

#endif // !MODEL