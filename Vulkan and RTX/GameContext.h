#ifndef INPUT_HANDLER
#define INPUT_HANDLER

struct GameContext
{
	std::unordered_map<uint32_t, bool> keyboardKeys;
	std::unordered_map<uint32_t, bool> mouseKeys;

	GameState gameState = GameState::IN_GAME;
	GameState requestedState = GameState::NONE;

	void clearInputs();
};

#endif