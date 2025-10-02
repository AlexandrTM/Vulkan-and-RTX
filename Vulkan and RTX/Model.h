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
    VkImage                                 image         = VK_NULL_HANDLE;
    VmaAllocation                           vmaAllocation = VK_NULL_HANDLE;
    VkImageView                             imageView     = VK_NULL_HANDLE;
    VkSampler                               sampler       = VK_NULL_HANDLE;
    uint32_t                                mipLevels     = 0;
    uint32_t                                width         = 0;
    uint32_t                                height        = 0;

    uint64_t hash = 0;

    Texture() { hash = randomHash64(); }

	explicit operator bool() const {
		return 
            image         != VK_NULL_HANDLE &&
            vmaAllocation != VK_NULL_HANDLE &&
			imageView     != VK_NULL_HANDLE &&
			sampler       != VK_NULL_HANDLE &&
            mipLevels     != 0 &&
            width         != 0 &&
            height        != 0 &&
            hash          != 0;
	}
};

struct Material {
    Texture                                 diffuseTexture; // basic color
    Texture                                 normalTexture;
    Texture                                 specularTexture;
    Texture                                 emissiveTexture;
};

struct Cuboid
{
    glm::vec3 min;
    glm::vec3 max;

    Cuboid operator+(const glm::vec3& offset) const
    {
        return Cuboid{ min + offset, max + offset };
    }
};

struct Mesh
{
    std::vector<Vertex>                     vertices;
    std::vector<uint32_t>                   indices;
    glm::mat4                               transform              = glm::mat4(1.0f);
    Cuboid                                  aabb;

    Material                                material;
    std::vector<Bone>                       bones;
    std::unordered_map<std::string, size_t> boneMap;

    VkBuffer                                vertexBuffer           = VK_NULL_HANDLE;
    VmaAllocation                           vertexBufferAllocation = VK_NULL_HANDLE;
    VkBuffer                                indexBuffer            = VK_NULL_HANDLE;
    VmaAllocation                           indexBufferAllocation  = VK_NULL_HANDLE;

    std::vector<VkDescriptorSet>            descriptorSets         = std::vector<VkDescriptorSet>(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);

    std::vector<VkBuffer>                   UBOBuffers             = std::vector<VkBuffer>       (MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    std::vector<VmaAllocation>              UBOAllocations         = std::vector<VmaAllocation>  (MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    std::vector<VkBuffer>		            boneSSBOBuffers        = std::vector<VkBuffer>       (MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
    std::vector<VmaAllocation>              boneSSBOAllocations    = std::vector<VmaAllocation>  (MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);
};

enum class ModelFlags : uint32_t
{
    NONE = 0,
    NEED_VERTEX = 1 << 0,
    NEED_INDEX = 1 << 1,
    NEED_AABB = 1 << 2,
    NEED_SHADERBUF = 1 << 3,
};

inline ModelFlags operator|(ModelFlags a, ModelFlags b) {
    return static_cast<ModelFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline bool hasFlag(ModelFlags value, ModelFlags flag) {
    return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

struct Model {
    std::vector<Mesh>                       meshes;
    ModelType                               type = ModelType::OTHER;
    ModelFlags flags =
        ModelFlags::NEED_VERTEX |
        ModelFlags::NEED_INDEX |
        ModelFlags::NEED_AABB |
        ModelFlags::NEED_SHADERBUF;

    glm::vec3                               position;
    glm::vec3                               scale;
    glm::quat                               rotation;

    // bounding box for frustum culling
    Cuboid                                  bounds;

    std::vector<std::vector<Mesh>>          lodLevels;

    std::vector<Animation>                  animations;

    bool                                    isCollidable = false;
    bool                                    isLoaded = false;  // Dynamic loading flag
};

void computeAABB(Mesh& mesh);

#endif // !MODEL