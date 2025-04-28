#ifndef GAME_CONTEXT
#define GAME_CONTEXT

#include "DungeonComponents.h"

struct GameContext
{
	std::unordered_map<uint32_t, bool> keyboardKeys;
	std::unordered_map<uint32_t, bool> mouseKeys;

	QPoint windowCenterPos;

	GameState currentGameState = GameState::NONE;
	GameState requestedGameState = GameState::NONE;

	void clearInputs();

	DungeonFloor dungeonFloor;
	DungeonRoom* currentRoom = nullptr;
	bool roomMovementHandled = false;

	std::vector<Equation> equations;
	int32_t selectedEquation = std::numeric_limits<int32_t>::max();
};

#endif