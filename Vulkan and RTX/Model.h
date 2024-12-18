#include "pch.h"
#include "Vertex.h"

#ifndef MODEL
#define MODEL

// Animation Key Types
struct PositionKey {
    double                                  time;
    glm::vec3                               position;
};

struct RotationKey {
    double                                  time;
    glm::quat                               rotation;
};

struct ScalingKey {
    double                                  time;
    glm::vec3                               scale;
};

// Animation Channel: Per-node animation data
struct AnimationChannel {
    std::string                             nodeName;         // The name of the node being animated
    std::vector<PositionKey>                positionKeys;
    std::vector<RotationKey>                rotationKeys;
    std::vector<ScalingKey>                 scalingKeys;
};

// Animation Data
struct Animation {
    std::string                             name;             // Name of the animation
    double                                  duration;         // Duration of the animation in ticks
    double                                  ticksPerSecond;   // Ticks per second (default: 25 if 0)

    std::vector<AnimationChannel>           channels;         // Animation channels for nodes
};

struct VertexWeight {
    uint32_t                                vertexID;
    float                                   weight;
};

struct Bone {
    std::string                             name;
    glm::mat4                               offsetMatrix;     // Bone to local space
    std::vector<VertexWeight>               weights;
};

struct Mesh {
    std::vector<Vertex>                     vertices;
    std::vector<uint32_t>                   indices;
    glm::mat4                               transform = glm::mat4(1.0f);

    VkBuffer                                vertexBuffer;
    VkDeviceMemory                          vertexBufferMemory;
    VkBuffer                                indexBuffer;
    VkDeviceMemory                          indexBufferMemory;
};

struct Texture {
    VkImage                                 image;
    VkDeviceMemory                          imageMemory;
    VkImageView                             imageView;
    VkSampler                               sampler;
    uint32_t                                mipLevels;
};

struct Material {
    Texture                                 diffuseTexture;
    Texture                                 normalTexture;
    Texture                                 specularTexture;
    Texture                                 emissiveTexture;
};

struct Model {
    std::vector<Mesh>                       meshes;
    std::vector<Material>                   materials;
    glm::vec3                               position;
    glm::vec3                               scale;
    glm::quat                               rotation;

    std::vector<Bone>                       bones;
    std::unordered_map<std::string, size_t> boneMap;

    // bounding box for frustum culling
    glm::vec3                               minBounds;
    glm::vec3                               maxBounds;

    std::vector<std::vector<Mesh>>          lodLevels;

    std::vector<Animation>                  animations;

    bool                                    isLoaded = false;  // Dynamic loading flag
};

#endif // !MODEL