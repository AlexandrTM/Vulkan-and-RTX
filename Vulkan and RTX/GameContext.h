#ifndef INPUT_HANDLER
#define INPUT_HANDLER

struct GameContext
{
	std::unordered_map<uint32_t, bool> keyboardKeys;
	std::unordered_map<uint32_t, bool> mouseKeys;

	GameState currentGameState = GameState::NONE;
	GameState requestedGameState = GameState::NONE;

	void clearInputs();
};

#endif