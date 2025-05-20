#include "pch.h"
#include "Dungeon.h"

DungeonRoom::DungeonRoom(
    glm::vec3 position,
    glm::ivec2 gridPosition,
    std::vector<std::string> layout,
    RoomConnectionMask connectionMask,
    float cellSize,
    Texture& floorTexture,
    Texture& wallTexture
)
    :
    position(position),
    gridPosition(gridPosition),
    layout(layout),
    connectionMask(connectionMask),
    cellSize(cellSize),
    floorTexture(floorTexture),
    wallTexture(wallTexture)
{
    metricWidth = layout.size() * cellSize;
    metricLength = layout[0].length() * cellSize;

    centerPosition = position + glm::vec3(metricWidth / 2.0f, 0.0f, metricLength / 2.0f);
    //std::cout << "center position: " << glm::to_string(centerPosition) << "\n";

    float verticalFovDegrees = 63.0f;
    float aspectRatio = 16.0f / 9.0f;

    float maxSide = std::max(metricWidth, metricLength);

    // Convert FOV to radians and get tangent
    float fovRadians = glm::radians(verticalFovDegrees);
    float halfFovTan = std::tan(fovRadians / 2.0f);

    // Distance from the room center along the Y axis to see full height
    float distanceY = (maxSide / 2.0f) / halfFovTan;

    // Optionally pad it a bit
    float zoomPadding = 1.45f;
    distanceY *= zoomPadding;

    // Use angle to compute final camera position (if camera is tilted)
    float pitchRadians = glm::radians(40.0f);
    float height = distanceY * std::sin(pitchRadians);
    float backOffset = distanceY * std::cos(pitchRadians);

    // Final camera position
    cameraPosition = centerPosition + glm::vec3(-backOffset, height, 0.0f);
}

std::vector<Model> DungeonRoom::createDungeonRoomModels() {
    std::vector<Model> models{};
    // Generate walls based on the room layout
    for (size_t x = 0; x < layout.size(); ++x) {
        for (size_t y = 0; y < layout[x].length(); ++y) {
            if (layout[x][y] == 'w') {
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
            else if (layout[x][y] == 'a') {
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
        metricWidth,
        0.001f,
        metricLength,
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
        layout[length - 1][midY] = 'a';
    }
    if (hasConnection(mask, RoomConnectionMask::SOUTH)) {
        layout[0][midY] = 'a';
    }
    if (hasConnection(mask, RoomConnectionMask::WEST)) {
        layout[midX][width - 1] = 'a';
    }
    if (hasConnection(mask, RoomConnectionMask::EAST)) {
        layout[midX][0] = 'a';
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

bool Dungeon::isDungeonFloorClear(const DungeonFloor& floor) {
	for (const auto& room : floor.dungeonRooms) {
		if (room.hasMobs()) {
			return false;
		}
	}
	return true;
}

QVariantList DungeonFloor::updateMinimapRoomList(DungeonRoom& currentRoom)
{
    QVariantList list;
    auto currentPos = currentRoom.gridPosition;

    for (const auto& room : this->dungeonRooms) {
        QVariantMap roomData;
        // flipped intentionally
        roomData["x"] = room.gridPosition.y;
        roomData["y"] = room.gridPosition.x;

        if (room.position == currentRoom.position)
            roomData["state"] = "current";
        else if (room.state == DungeonRoomState::DISCOVERED)
            roomData["state"] = "discovered";
        else
            roomData["state"] = "undiscovered";

        roomData["hasMob"] = room.hasMobs();

        list << roomData;
    }

    return list;
}

void Dungeon::updateRoomsState(
	DungeonRoom& targetRoom, 
	DungeonRoom& previousRoom, 
	std::vector<DungeonRoom>& floorRooms,
    int32_t viewDistance
)
{
	if (previousRoom.state == DungeonRoomState::CURRENT) {
		previousRoom.state = DungeonRoomState::DISCOVERED;
	}

	targetRoom.state = DungeonRoomState::CURRENT;

    for (const auto& [dir, offset] : directionOffsets) {
        glm::ivec2 neighborPos = targetRoom.gridPosition + offset;

        for (DungeonRoom& neighbor : floorRooms) {
            if (neighbor.gridPosition == neighborPos &&
                neighbor.state == DungeonRoomState::UNDISCOVERED) {
                neighbor.state = DungeonRoomState::DISCOVERED;
            }
        }
    }

    /*std::queue<std::pair<glm::ivec2, int32_t>> toVisit;
    std::vector<glm::ivec2> visited;

    toVisit.push({ targetRoom.gridPosition, 0 });
    visited.push_back(targetRoom.gridPosition);

    auto isVisited = [&](const glm::ivec2& pos) {
        return std::find(visited.begin(), visited.end(), pos) != visited.end();
        };

    auto findRoomAt = [&](const glm::ivec2& pos) -> DungeonRoom* {
        for (DungeonRoom& room : floorRooms) {
            if (room.gridPosition == pos) return &room;
        }
        return nullptr;
        };

    while (!toVisit.empty()) {
        auto& [gridPosition, distance] = toVisit.front();
        toVisit.pop();

        if (distance >= viewDistance) continue;

        DungeonRoom* currentRoom = findRoomAt(gridPosition);
        if (!currentRoom) continue;

        for (const auto& [dir, offset] : directionOffsets) {
            glm::ivec2 neighborPos = gridPosition + offset;

            if (!hasConnection(currentRoom->connectionMask, dir)) continue;

            DungeonRoom* neighbor = findRoomAt(neighborPos);
            if (!neighbor || isVisited(neighborPos)) continue;

            if (hasConnection(neighbor->connectionMask, oppositeDirection(dir))) {
                if (neighbor->state == DungeonRoomState::UNDISCOVERED) {
                    neighbor->state = DungeonRoomState::DISCOVERED;
                }
                toVisit.push({ neighborPos, distance + 1 });
                visited.push_back(neighborPos);
            }
        }
    }*/
}

std::vector<Model> Dungeon::generateRandomDungeonFloor(
	int32_t floorNumber, float difficultyScale, 
	DungeonFloor& dungeonFloor,
	Texture& floorTexture, Texture& wallTexture
) {
	std::unordered_map<glm::ivec2, RoomInfo> roomGrid;

	size_t minRoomCount = 15;
	size_t maxRoomCount = 15;
	float cellSize = 1.0f;
	size_t maxRoomDimension = 9; // max of width or height
	float roomSpacing = (maxRoomDimension * cellSize) + cellSize * 10;

	generateDungeonFloorGrid(minRoomCount, maxRoomCount, roomGrid);

	createDungeonRoomsFromGrid(
		dungeonFloor, roomGrid, 
		cellSize, roomSpacing, 
		floorTexture, wallTexture
	);

	return dungeonFloor.createDungeonFloor(floorNumber, difficultyScale);
}

void Dungeon::generateDungeonFloorGrid(
	size_t& minRoomCount, 
	size_t& maxRoomCount, 
	std::unordered_map<glm::ivec2, RoomInfo>& roomGrid
)
{
	std::queue<glm::ivec2> frontier;

	while (roomGrid.size() < minRoomCount) {
        size_t roomWidth = randomOddInt(9, 13);
        size_t roomLength = roomWidth;

		// clean generation if failed
		frontier.push({ 0, 0 });
		roomGrid.clear();
		roomGrid[{0, 0}] = { RoomConnectionMask::NONE, roomWidth, roomLength };
		std::vector<std::pair<RoomConnectionMask, glm::ivec2>> shuffledOffsets(
			directionOffsets.begin(),
			directionOffsets.end()
		);
		std::shuffle(shuffledOffsets.begin(), shuffledOffsets.end(), gen);
		size_t neighborChance = 15;

		// try generate dungeon floor layout
		while (!frontier.empty()) {
			glm::ivec2 current = frontier.front(); 
            frontier.pop();
			RoomConnectionMask mask = RoomConnectionMask::NONE;

			neighborChance = glm::clamp(
				10 + roomGrid.size() * 2,
				static_cast<size_t>(5), static_cast<size_t>(40)
			);

			for (auto& [dir, offset] : directionOffsets) {
				glm::ivec2 neighbor = current + offset;

				// Randomly decide to add neighbor room
				if (roomGrid.size() < maxRoomCount &&
					roomGrid.find(neighbor) == roomGrid.end() &&
					(randomInt(0, 99) < neighborChance)) {

                    roomWidth = randomOddInt(9, 13);
                    roomLength = roomWidth;
					roomGrid[neighbor] = { oppositeDirection(dir), roomWidth, roomLength };
                    //std::cout << "room grid coords: " << glm::to_string(neighbor) << "\n";
					mask = static_cast<RoomConnectionMask>(mask | dir);
					frontier.push(neighbor);
				}
				// Already placed neighbor, ensure bi-directional connection
				/*else if (roomGrid.find(neighbor) != roomGrid.end()) {
					roomGrid[neighbor] =
						static_cast<RoomConnectionMask>(roomGrid[neighbor] | oppositeDirection(dir));
					mask = static_cast<RoomConnectionMask>(mask | dir);
				}*/
			}

			roomGrid[current].connectionMask = 
                static_cast<RoomConnectionMask>(roomGrid[current].connectionMask | mask);
			//std::cout << "room count: " << roomGrid.size() << "\n";
		}
	}
}

void Dungeon::createDungeonRoomsFromGrid(
	DungeonFloor& dungeonFloor, 
	std::unordered_map<glm::ivec2, RoomInfo>& roomGrid,
	float& cellSize, float& roomSpacing,
	Texture& floorTexture, Texture& wallTexture
)
{
	for (const auto& [gridPosition, roomInfo] : roomGrid) {
		glm::vec3 worldPosition = glm::vec3(gridPosition.x * roomSpacing, 0.0f, gridPosition.y * roomSpacing);

		std::vector<std::string> layout = DungeonRoom::createRoomLayoutFromMask(
            roomInfo.connectionMask, roomInfo.width, roomInfo.length
        );

		dungeonFloor.addDungeonRoom(DungeonRoom(
			worldPosition,
			gridPosition,
			layout,
			roomInfo.connectionMask,
			cellSize,
			floorTexture,
			wallTexture
		));
	}
}
