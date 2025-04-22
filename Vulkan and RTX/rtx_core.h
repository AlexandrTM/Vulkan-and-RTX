#ifndef RTX_CORE_H
#define RTX_CORE_H

constexpr uint32_t MAX_BONES_NUM = 256;
constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

enum class PipelineType : uint32_t
{
	OBJECT = 0,
	SKY = 1,
	GUI = 2,
};

enum class Gamemode : uint32_t
{
	CREATIVE = 0,
	SURVIVAL = 1,
};

enum class GameState : uint32_t
{
	MAIN_MENU = 0,
	SETTINGS_MENU = 1,
	IN_GAME = 2,
	IN_DUNGEON = 3,
	COMBAT = 4,
	SHOP = 5,
	PAUSED = 6,
	EXIT = 7,
	NONE = std::numeric_limits<uint32_t>::max(),
};

struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 projection;
	alignas(16) glm::vec3 sun;
	alignas(16) glm::vec3 observer;
	alignas(4)  float visibilityRange = 6000.0f;
};

struct ShaderStorageBufferObject
{
	alignas(16) std::array<glm::mat4, MAX_BONES_NUM> boneTransforms = { glm::mat4(1.0f) };
};

#endif
 