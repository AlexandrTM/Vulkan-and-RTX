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

enum class Gamemode : uint32_t
{
	CREATIVE = 0,
	SURVIVAL = 1,
};

enum class EquationDifficulty : uint32_t
{
	EASY = 0,
	MEDIUM = 1,
	HARD = 2,
	INSANE = 3,
	BEYOND = 4,
	UNKNOWN = std::numeric_limits<uint32_t>::max(),
};

struct Equation
{
	std::string expression;
	int32_t difficulty;
	int32_t damage;
};

extern std::default_random_engine gen;
//extern boost::random::mt19937 genenerator;
void seedRandomGenerator();
int32_t randomInt(int32_t min, int32_t max);
float randomNormalizedReal();

enum class GameState : uint32_t
{
	MAIN_MENU = 0,
	SETTINGS_MENU = 1,
	PAUSED = 2,
	IN_GAME_TESTING = 3, // for testing purposes only
	DUNGEON_EXPLORATION = 4,
	COMBAT_PLAYER_SELECT_EQUATION = 5,
	COMBAT_PLAYER_SOLVE_EQUATION = 6,
	COMBAT_MOB_TURN = 7,
	SHOP = 8,
	HALL_OF_FAME = 9,
	GAME_OVER = 10,
	EXIT = std::numeric_limits<uint32_t>::max() - 1,
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
 