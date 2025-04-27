#include "pch.h"
#include "Dungeon.h"

void Dungeon::createDungeonFloor(
	DungeonFloor& dungeonFloor, std::vector<Model>& models,
	Texture& floorTexture, Texture& wallTexture
) {
	std::unordered_map<glm::ivec2, RoomConnectionMask> roomGrid;

	size_t minRoomCount = 15;
	size_t maxRoomCount = 15;
	float cellSize = 2.0f;
	size_t maxRoomDimension = 9; // max of width or height
	float roomSpacing = (maxRoomDimension * cellSize) + cellSize * 10;

	generateDungeonFloorGrid(minRoomCount, maxRoomCount, roomGrid);

	createDungeonRoomsFromGrid(
		dungeonFloor, roomGrid, 
		cellSize, roomSpacing, 
		floorTexture, wallTexture
	);

	dungeonFloor.createDungeonFloor(models);
}
void Dungeon::enterDungeonFloor(
	DungeonFloor& dungeonFloor, GameContext& gameContext, Character& character
)
{
	gameContext.currentRoom = dungeonFloor.entrance;
	if (gameContext.currentRoom != nullptr) {
		character.camera.setPosition(dungeonFloor.entrance->cameraPosition);
	}
	else {
		std::cout << "there is no entrance room for this dungeon floor\n";
	}
}

void Dungeon::generateDungeonFloorGrid(
	size_t& minRoomCount, 
	size_t& maxRoomCount, 
	std::unordered_map<glm::ivec2, RoomConnectionMask>& roomGrid
)
{
	std::queue<glm::ivec2> frontier;

	while (roomGrid.size() < minRoomCount) {
		// clean generation if failed
		frontier.push({ 0, 0 });
		roomGrid.clear();
		roomGrid[{0, 0}] = RoomConnectionMask::NONE;
		std::vector<std::pair<RoomConnectionMask, glm::ivec2>> shuffledOffsets(
			directionOffsets.begin(),
			directionOffsets.end()
		);
		std::shuffle(shuffledOffsets.begin(), shuffledOffsets.end(), std::default_random_engine(1));
		size_t neighborChance = 15;

		// try generate dungeon floor layout
		while (!frontier.empty()) {
			glm::ivec2 current = frontier.front(); frontier.pop();
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
					(rand() % 100 < neighborChance)) {
					roomGrid[neighbor] = oppositeDirection(dir);
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

			roomGrid[current] = static_cast<RoomConnectionMask>(roomGrid[current] | mask);
			//std::cout << "room count: " << roomGrid.size() << "\n";
		}
	}
}

void Dungeon::createDungeonRoomsFromGrid(
	DungeonFloor& dungeonFloor, 
	std::unordered_map<glm::ivec2, RoomConnectionMask>& roomGrid,
	float& cellSize, float& roomSpacing,
	Texture& floorTexture, Texture& wallTexture
)
{
	for (auto& [gridPosition, connectionMask] : roomGrid) {
		glm::vec3 worldPosition = glm::vec3(gridPosition.x * roomSpacing, 0.0f, gridPosition.y * roomSpacing);
		//size_t roomWidth = (rand() % 3) * 2 + 7; // 7,9,11
		//size_t roomHeight = (rand() % 3) * 2 + 7;

		size_t roomWidth = 9;
		size_t roomLength = 9;

		std::vector<std::string> layout = DungeonRoom::createRoomLayoutFromMask(connectionMask, roomWidth, roomLength);

		dungeonFloor.addDungeonRoom(DungeonRoom(
			worldPosition,
			gridPosition,
			layout,
			connectionMask,
			cellSize,
			floorTexture,
			wallTexture
		));
	}
}
