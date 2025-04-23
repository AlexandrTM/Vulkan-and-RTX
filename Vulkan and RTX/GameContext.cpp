#include "pch.h"
#include "GameContext.h"

void GameContext::clearInputs() {
	/*keyboardKeys.clear();*/
	for (auto& pair : keyboardKeys) {
		pair.second = false;
	}
	mouseKeys.clear();
}
