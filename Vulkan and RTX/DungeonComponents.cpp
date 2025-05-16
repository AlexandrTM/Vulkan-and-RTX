#include "pch.h"
#include "DungeonComponents.h"

DungeonRoom::DungeonRoom(
    glm::vec3 position,
    glm::ivec2 gridPosition,
    std::vector<std::string> roomLayout,
    RoomConnectionMask connectionMask,
    float cellSize,
    Texture& floorTexture,
    Texture& wallTexture
)
    :
    position(position),
    gridPosition(gridPosition),
    roomLayout(roomLayout),
    connectionMask(connectionMask),
    cellSize(cellSize),
    floorTexture(floorTexture),
    wallTexture(wallTexture)
{
    metricWidth = roomLayout[0].length() * cellSize;
    metricLength = roomLayout.size() * cellSize;

    centerPosition = position + glm::vec3(metricWidth / 2.0f, 0.0f, metricLength / 2.0f);
    //std::cout << "center position: " << glm::to_string(centerPosition) << "\n";

    float_t zoomCoefficient = 1.45;
    cameraPosition = centerPosition + glm::vec3(-8.3 * zoomCoefficient, 7.5 * zoomCoefficient, 0.0);
}

std::vector<Model> DungeonRoom::createDungeonRoomModels() {
    std::vector<Model> models;
    // Generate walls based on the room layout
    for (size_t x = 0; x < roomLayout.size(); ++x) {
        for (size_t y = 0; y < roomLayout[x].length(); ++y) {
            if (roomLayout[x][y] == 'w') {
                // Place wall cubes
                models.push_back(ModelManager::createCube(
                    position.x + x * cellSize,
                    position.y,
                    position.z + y * cellSize,
                    cellSize,
                    glm::vec3(0.5f),
                    wallTexture,
                    ModelType::DUNGEON
                ));
            }
            else if (roomLayout[x][y] == 'a') {
                // Empty space (air), skip rendering
                continue;
            }
        }
    }

    // Create the floor of the room
    models.push_back(ModelManager::createCuboid(
        position.x,
        position.y - 0.001f,
        position.z,
        metricLength,
        0.001f,
        metricWidth,
        glm::vec3(1.0f),
        floorTexture,
        ModelType::DUNGEON
    ));

    return models;
}

std::vector<std::string> DungeonRoom::createRoomLayoutFromMask(
    RoomConnectionMask mask,
    size_t width, size_t length
)
{
    std::vector<std::string> layout(length, std::string(width, 'a'));

    // Place walls on borders
    for (size_t x = 0; x < length; ++x) {
        for (size_t y = 0; y < width; ++y) {
            if (x == 0 || x == length - 1 || y == 0 || y == width - 1) {
                layout[x][y] = 'w';
            }
        }
    }

    size_t midX = length / 2;
    size_t midY = width / 2;

    // Open passages based on mask
    if (hasConnection(mask, RoomConnectionMask::NORTH)) {
        layout[0][midY] = 'a';
    }
    if (hasConnection(mask, RoomConnectionMask::SOUTH)) {
        layout[length - 1][midY] = 'a';
    }
    if (hasConnection(mask, RoomConnectionMask::WEST)) {
        layout[midX][0] = 'a';
    }
    if (hasConnection(mask, RoomConnectionMask::EAST)) {
        layout[midX][width - 1] = 'a';
    }

    return layout;
}

// Method to generate all rooms on the floor
std::vector<Model> DungeonFloor::createDungeonFloor(int32_t floorNumber, float difficultyScale) {
    std::vector<Model> floorModels;

    if (!dungeonRooms.empty()) {
        entrance = &dungeonRooms.front();
    }
    else {
        std::cout << "dungeon floor has no rooms\n";
    }

    // Generate each room in the dungeon
    for (auto& room : dungeonRooms) {
        std::vector<Model> roomModels = room.createDungeonRoomModels();

        floorModels.insert(floorModels.end(), roomModels.begin(), roomModels.end());

        if (&room != entrance) {
            room.mobs.push_back(Mob::generateRandomMob(room.centerPosition, floorNumber, difficultyScale));
        }
    }
    return floorModels;
}

void DungeonFloor::addDungeonRoom(DungeonRoom dungeonRoom) {
    dungeonRooms.push_back(dungeonRoom);
}
