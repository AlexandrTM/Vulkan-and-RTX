#pragma once

#include "DungeonComponents.h"
#include "Equations.h"

struct GameContext
{
	std::unordered_map<uint32_t, bool> keyboardKeys;
	std::unordered_map<uint32_t, bool> mouseKeys;

	QPoint windowCenterPos;

	GameState currentGameState = GameState::NONE;
	GameState requestedGameState = GameState::NONE;

	void clearInputs();

	DungeonFloor dungeonFloor;
	int32_t currentFloor = 0;
	DungeonRoom* currentRoom = nullptr;
	DungeonRoom* targetRoom = nullptr;
	bool isRoomMovementHandled = false;

	std::vector<Equation> equations;
	Equation* selectedEquation = nullptr;
	double timeRemainingToSolveEquation = 0.0;
	bool isAnswerSubmitted = false;

	bool isCameraTransitioning = false;
	glm::vec3 cameraStartPosition;
	glm::vec3 cameraTargetPosition;
	float cameraCurrentTransitionTime = 0.0f;
	const float cameraTransitionDuration = 0.5f;
};
