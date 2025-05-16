#ifndef DUNGEON_CORE_H
#define DUNGEON_CORE_H

enum class RoomConnectionMask : uint32_t
{
    NONE = 0,
    NORTH = 1 << 0,
    EAST = 1 << 1,
    SOUTH = 1 << 2,
    WEST = 1 << 3
};

enum class DungeonRoomState : uint32_t
{
    UNDISCOVERED = 0,
    DISCOVERED = 1,
    CURRENT = 2,
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
    { RoomConnectionMask::NORTH, { -1,  0 } },
    { RoomConnectionMask::SOUTH, {  1,  0 } },
    { RoomConnectionMask::EAST,  {  0,  1 } },
    { RoomConnectionMask::WEST,  {  0, -1 } },
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

#endif // !DUNGEON_CORE_H