#pragma once

#include "Model.h"
#include "Mob.h"
#include "ModelManager.h"

struct DungeonRoom
{
    glm::vec3 position;
    glm::ivec2 gridPosition;

    float cellSize;
    float metricWidth;
    float metricLength;
    glm::vec3 centerPosition;
    glm::vec3 cameraPosition;

    std::vector<std::string> layout;
    RoomConnectionMask connectionMask;
    DungeonRoomState state = DungeonRoomState::UNDISCOVERED;

    Texture floorTexture;
    Texture wallTexture;

    std::vector<Mob> mobs;

    bool hasMobs() const { return !mobs.empty(); }

    DungeonRoom(
        glm::vec3 position,
        glm::ivec2 gridPosition,
        std::vector<std::string> layout,
        RoomConnectionMask connectionMask,
        float cellSize,
        Texture& floorTexture,
        Texture& wallTexture
    );

    std::vector<Model> createDungeonRoomModels();
    static std::vector<std::string> createRoomLayoutFromMask(
        RoomConnectionMask mask,
        size_t width, size_t length
    );
};

struct DungeonFloor
{
    std::vector<DungeonRoom> dungeonRooms;
    DungeonRoom* entrance = nullptr;

    std::vector<Model> createDungeonFloor(int32_t floorNumber, float difficultyScale);

    void addDungeonRoom(DungeonRoom dungeonRoom);
};

namespace Dungeon
{
	bool isDungeonFloorClear(const DungeonFloor& floor);

	void updateRoomsState(
		DungeonRoom& targetRoom,
		DungeonRoom& previousRoom,
		std::vector<DungeonRoom>& floorRooms
	);

	std::vector<Model> createDungeonFloor(
		int32_t floorNumber, float difficultyScale,
		DungeonFloor& dungeonFloor,
		Texture& floorTexture, Texture& wallTexture
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