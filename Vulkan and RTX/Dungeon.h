#pragma once

#include "Model.h"
#include "Mob.h"
#include "ModelPrimitives.h"

struct RoomInfo
{
    RoomConnectionMask connectionMask;
    size_t width;
    size_t length;
};

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
};

struct DungeonFloor
{
    std::vector<DungeonRoom> dungeonRooms;
    DungeonRoom* entrance = nullptr;

    QVariantList updateMinimapRoomList(DungeonRoom& currentRoom);
    std::vector<Model> createDungeonFloorModels();
    void setEntrance();

    void addDungeonRoom(DungeonRoom dungeonRoom);
};

namespace Dungeon
{
	bool isDungeonFloorClear(const DungeonFloor& floor);

	void updateRoomsState(
		DungeonRoom& targetRoom,
		DungeonRoom& previousRoom,
		std::vector<DungeonRoom>& floorRooms,
        int32_t viewDistance
	);

	std::vector<Model> generateRandomDungeonFloor(
		int32_t floorNumber, float difficultyScale,
		DungeonFloor& dungeonFloor,
		Texture& floorTexture, Texture& wallTexture
	);

	void generateDungeonFloorGrid(
		size_t& minRoomCount,
		size_t& maxRoomCount,
		std::unordered_map<glm::ivec2, RoomInfo>& roomGrid
	);
    void generateDungeonFloorMobs(
        DungeonFloor& dungeonFloor, int32_t floorNumber, float difficultyScale
    );

	void createDungeonRoomsFromGrid(
		DungeonFloor& dungeonFloor,
		std::unordered_map<glm::ivec2, RoomInfo>& roomGrid,
		float& cellSize, float& roomSpacing,
		Texture& floorTexture, Texture& wallTexture
	);

    std::vector<std::string> createDungeonRoomLayout(const RoomInfo& roomInfo);
}