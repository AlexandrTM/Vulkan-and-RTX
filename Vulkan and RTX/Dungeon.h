#pragma once

#include "Model.h"
#include "DungeonComponents.h"
#include "GameContext.h"
#include "Character.h"

namespace Dungeon
{
	void createDungeonFloor(
		DungeonFloor& dungeonFloor, std::vector<Model>& models,
		Texture& floorTexture, Texture& wallTexture
	);
	void enterDungeonFloor(
		DungeonFloor& dungeonFloor, GameContext& gameContext, Character& character
	);

	void generateDungeonFloorGrid(
		size_t& minRoomCount,
		size_t& maxRoomCount,
		std::unordered_map<glm::ivec2, RoomConnectionMask>& roomGrid
	);

	void createDungeonRoomsFromGrid(
		DungeonFloor& dungeonFloor,
		std::unordered_map<glm::ivec2, RoomConnectionMask>& roomGrid,
		float& cellSize, float& roomSpacing,
		Texture& floorTexture, Texture& wallTexture
	);
}