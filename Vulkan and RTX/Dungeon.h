#pragma once

#include "Model.h"
#include "DungeonComponents.h"
#include "Character.h"

namespace Dungeon
{
	bool isDungeonFloorCleaned(const DungeonFloor& floor);

	std::vector<Model> createDungeonFloor(
		int32_t floorNumber, float difficultyScale,
		DungeonFloor& dungeonFloor,
		Texture& floorTexture, Texture& wallTexture
	);
	DungeonRoom* enterDungeonFloor(
		DungeonFloor& dungeonFloor, Character& character
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