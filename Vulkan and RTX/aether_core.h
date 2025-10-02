#ifndef AETHER_CORE_H
#define AETHER_CORE_H

#include <boost/random.hpp>

constexpr uint32_t MAX_BONES_NUM = 256;
constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

enum class PipelineType : uint32_t
{
	OBJECT = 0,
	SKY = 1,
	GUI = 2,
};

enum class VertexLayoutType : uint32_t
{
	WHOLE = 0,
	POSITION_ONLY = 1,
	POSITION_TEXCOORDS = 2,
};

enum class ModelType : uint32_t
{
	DUNGEON = 0,
	OTHER = std::numeric_limits<uint32_t>::max(),
};

enum class Gamemode : uint32_t
{
	CREATIVE = 0,
	SURVIVAL = 1,
};

extern boost::random::mt19937 generator_32;
extern boost::random::mt19937_64 generator_64;
void seedRandomGenerator();

template<typename IntType>
typename std::enable_if<std::is_integral<IntType>::value, IntType>::type
randomInt(IntType min, IntType max) {
	boost::random::uniform_int_distribution<IntType> distribution(min, max);
	return distribution(generator_32);
}
template<typename IntType>
typename std::enable_if<std::is_integral<IntType>::value, IntType>::type
randomOddInt(IntType min, IntType max) {
	// make odd numbers
	if (min % 2 == 0) min += 1;
	if (max % 2 == 0) max -= 1;

	return randomInt(min / 2, max / 2) * 2 + 1;
}
template<typename IntType>
typename std::enable_if<std::is_integral<IntType>::value, IntType>::type
randomEvenInt(IntType min, IntType max) {
	// make even numbers
	if (min % 2 != 0) min += 1;
	if (max % 2 != 0) max -= 1;

	return randomInt(min / 2, max / 2) * 2;
}
float randomReal(float min, float max);
float randomRealOffset(float offset);
float randomNormalizedReal();
uint64_t randomHash64();
float randomNormalizedWeightedReal(std::vector<float>& weights);

enum class GameState : uint32_t
{
	MAIN_MENU,
	SETTINGS_MENU,
	PAUSED,
	IN_GAME_TESTING, // for testing purposes only

	WORLD_EXPLORATION,

	ENTERING_DUNGEON,
	DUNGEON_EXPLORATION,
	DUNGEON_ROOM_CLEANED,
	LEAVING_DUNGEON,

	COMBAT_PLAYER_SELECT_EQUATION,
	COMBAT_PLAYER_SOLVE_EQUATION,
	COMBAT_MOB_TURN,

	SHOP,
	HALL_OF_FAME,
	PLAYER_DEAD,

	EXIT = std::numeric_limits<uint32_t>::max() - 1,
	NONE = std::numeric_limits<uint32_t>::max(),
};

enum class uiElementId : uint32_t
{
	MainMenu,
	SettingsMenu,
	PauseMenu,
	InGameOverlay,
	SelectEquation,
	SolveEquation,
	Count
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
 