#include "pch.h"
#include "GameContext.h"

void GameContext::clearInputs() {
	for (auto& pair : keyboardKeys) {
		pair.second = false;
	}
	for (auto& pair : mouseKeys) {
		pair.second = false;
	}
	//std::cout << "inputs cleared\n";
	//keyboardKeys.clear();
	//mouseKeys.clear();
}
