#include "pch.h"
#include "Vertex.h"

#ifndef MODEL
#define MODEL

static const uint32_t BONES_NUM = 256;
static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

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
    glm::mat4                               offsetMatrix;   // Transform from mesh space to bone local space
    glm::mat4                               finalTransform; // Final transformation for skinning
    glm::mat4                               globalTransform; // Local transformation (updated by animations)
    std::vector<int>                        children; // Indices of child bones
};

struct Texture {
    VkImage                                 image       = VK_NULL_HANDLE;
    VkDeviceMemory                          imageMemory = VK_NULL_HANDLE;
    VkImageView                             imageView   = VK_NULL_HANDLE;
    VkSampler                               sampler     = VK_NULL_HANDLE;
    uint32_t                                mipLevels = 0;

	explicit operator bool() const {
		return 
            image       != VK_NULL_HANDLE &&
			imageMemory != VK_NULL_HANDLE &&
			imageView   != VK_NULL_HANDLE &&
			sampler     != VK_NULL_HANDLE;
	}
};

struct Material {
    Texture                                 diffuseTexture;
    Texture                                 normalTexture;
    Texture                                 specularTexture;
    Texture                                 emissiveTexture;
};

struct Cuboid
{
    glm::vec3 min;
    glm::vec3 max;
};

struct Mesh
{
    std::vector<Vertex>                     vertices;
    std::vector<uint32_t>                   indices;
    glm::mat4                               transform = glm::mat4(1.0f);
    Cuboid                                  aabb;

    VkBuffer                                vertexBuffer;
    VkDeviceMemory                          vertexBufferMemory;
    VkBuffer                                indexBuffer;
    VkDeviceMemory                          indexBufferMemory;

    std::vector<Bone>                       bones;
    std::unordered_map<std::string, size_t> boneMap;

    std::vector<VkDescriptorSet>            descriptorSets        = std::vector<VkDescriptorSet>(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);

    std::vector<VkBuffer>                   UBOBuffers            = std::vector<VkBuffer>(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    std::vector<VkDeviceMemory>             UBOBuffersMemory      = std::vector<VkDeviceMemory>(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    std::vector<VkBuffer>		            boneSSBOBuffers       = std::vector<VkBuffer>(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    std::vector<VkDeviceMemory>             boneSSBOBuffersMemory = std::vector<VkDeviceMemory>(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
};

struct Model {
    std::vector<Mesh>                       meshes;
    std::vector<Material>                   materials;
    glm::vec3                               position;
    glm::vec3                               scale;
    glm::quat                               rotation;

    // bounding box for frustum culling
    glm::vec3                               minBounds;
    glm::vec3                               maxBounds;

    std::vector<std::vector<Mesh>>          lodLevels;

    std::vector<Animation>                  animations;

    bool                                    isCollidable = false;
    bool                                    isLoaded = false;  // Dynamic loading flag
};

void computeAABB(Mesh& mesh);

#endif // !MODEL