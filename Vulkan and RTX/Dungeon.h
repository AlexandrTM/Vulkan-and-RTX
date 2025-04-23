#include "pch.h"

#ifndef DUNGEON_H
#define DUNGEON_H

#include "Model.h"

enum class RoomConnectionMask : uint32_t
{
    NONE  = 0,
    NORTH = 1 << 0,
    EAST  = 1 << 1,
    SOUTH = 1 << 2,
    WEST  = 1 << 3
};

inline RoomConnectionMask operator|(RoomConnectionMask a, RoomConnectionMask b) {
    return static_cast<RoomConnectionMask>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
        );
}

inline bool hasConnection(RoomConnectionMask mask, RoomConnectionMask direction) {
    return static_cast<uint32_t>(mask) & static_cast<uint32_t>(direction);
}

const std::unordered_map<RoomConnectionMask, glm::ivec2> directionOffsets = {
    { RoomConnectionMask::NORTH, { -1,  0} },
    { RoomConnectionMask::SOUTH, {  1,  0} },
    { RoomConnectionMask::EAST,  {  0,  1} },
    { RoomConnectionMask::WEST,  {  0, -1} },
};

inline RoomConnectionMask oppositeDirection(RoomConnectionMask dir) {
    switch (dir) {
    case RoomConnectionMask::NORTH: return RoomConnectionMask::SOUTH;
    case RoomConnectionMask::SOUTH: return RoomConnectionMask::NORTH;
    case RoomConnectionMask::EAST:  return RoomConnectionMask::WEST;
    case RoomConnectionMask::WEST:  return RoomConnectionMask::EAST;
    default: return RoomConnectionMask::NONE;
    }
}

struct DungeonRoom
{
    glm::vec3 position;
    float metricWidth;
    float metricLength;
    glm::vec3 centerPosition;
    glm::vec3 cameraPosition;

    std::vector<std::string> roomLayout;
    float cellSize;
    Texture floorTexture;
    Texture wallTexture;

    DungeonRoom(
        glm::vec3 position, 
        std::vector<std::string> roomLayout, 
        float cellSize,
        Texture& floorTexture, 
        Texture& wallTexture
    );

    void createRoom(std::vector<Model>& models);
    static std::vector<std::string> createRoomLayoutFromMask(
        RoomConnectionMask mask,
        size_t width, size_t length
    );
};

struct DungeonFloor
{
    std::vector<DungeonRoom> dungeonRooms;

    void createDungeonFloor(std::vector<Model>& models);

    void addDungeonRoom(DungeonRoom dungeonRoom);
};

#endif