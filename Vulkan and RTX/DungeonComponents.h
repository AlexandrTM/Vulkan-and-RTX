#include "pch.h"

#pragma once

#include "Model.h"
#include "ModelManager.h"
#include "Mob.h"

struct DungeonRoom
{
    glm::vec3 position;
    glm::ivec2 gridPosition;

    float cellSize;
    float metricWidth;
    float metricLength;
    glm::vec3 centerPosition;
    glm::vec3 cameraPosition;

    std::vector<std::string> roomLayout;
    RoomConnectionMask connectionMask;

    Texture floorTexture;
    Texture wallTexture;

    std::vector<Mob> mobs;

    DungeonRoom(
        glm::vec3 position,
        glm::ivec2 gridPosition,
        std::vector<std::string> roomLayout,
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

    std::vector<Model> createDungeonFloor();

    void addDungeonRoom(DungeonRoom dungeonRoom);
};

