#include "pch.h"
#include "Character.h"

void Character::handleInGamePlayerInput()
{
	if (gameContext.keyboardKeys[Qt::Key_Escape]) {
		gameContext.requestedGameState = GameState::PAUSED;
	}

	if (gameContext.keyboardKeys[Qt::Key_Up])
	{
		mouseSensitivity = std::clamp(mouseSensitivity * 1.25, 0.001, 10.0);
	}
	if (gameContext.keyboardKeys[Qt::Key_Down])
	{
		mouseSensitivity = std::clamp(mouseSensitivity * 0.8, 0.001, 10.0);
	}
	if (gameContext.mouseKeys[Qt::RightButton]) {
		camera.setVerticalFov(60.0f);
		mouseSensitivity = 0.125;
	}

	if (gameContext.keyboardKeys[Qt::Key_F]) {
		if (isInteracting == nullptr) {
			for (size_t i = 0; i < interactableCuboids.size(); i++) {
				if (interactableCuboids[i].rayIntersectsCuboid(
					camera.getPosition(), camera.getDirection())) {
					isInteracting = &interactableCuboids[i];
					isInteracting->isOpen = true;
				}
			}
		}
	}
	
	// changing mipmap level of detail
	/*if (keyboardKeys[Qt::Key_Left)
	{
		if (!(currentMinMipLevels < 0.0f))
		{
			currentMinMipLevels -= 1.0f;
		}
		if (currentMinMipLevels < 0.0f)
		{
			currentMinMipLevels = 0.0f;
		}
	}
	if (keyboardKeys[Qt::Key_Right)
	{
		if (!(currentMinMipLevels > 14.0f))
		{
			currentMinMipLevels += 1.0f;
		}
		if (currentMinMipLevels > 14.0f)
		{
			currentMinMipLevels = 14.0f;
		}
	}*/
}
void Character::handleDungeonExplorationPlayerInput()
{
	if (gameContext.keyboardKeys[Qt::Key_Escape]) {
		gameContext.requestedGameState = GameState::PAUSED;
	}

	handleDungeonRoomMovement();
}

void Character::handleDungeonRoomMovement() 
{
	if (gameContext.isCameraTransitioning) return;

	// Check if any movement key is pressed
	auto isKeyPressed = [&](const std::vector<Qt::Key>& keys) {
		return std::any_of(
			keys.begin(), keys.end(),
			[&](Qt::Key k) { return gameContext.keyboardKeys[k]; }
		);
	};

	std::vector<Qt::Key> movementKeys = {
		Qt::Key_W, Qt::Key_Up,
		Qt::Key_A, Qt::Key_Left,
		Qt::Key_S, Qt::Key_Down,
		Qt::Key_D, Qt::Key_Right
	};

	if (!isKeyPressed(movementKeys)) {
		gameContext.isRoomMovementHandled = false;
	}

	// ensure per key release movement	// ensure no pending movement, to not skip dungeon room
	if (gameContext.isRoomMovementHandled || gameContext.requestedGameState != GameState::NONE) {
		return;
	}

	// Movement mapping
	static const std::unordered_map<Qt::Key, RoomConnectionMask> keyToDirection = {
		{Qt::Key_W, RoomConnectionMask::NORTH}, {Qt::Key_Up, RoomConnectionMask::NORTH},
		{Qt::Key_A, RoomConnectionMask::EAST},  {Qt::Key_Left, RoomConnectionMask::EAST},
		{Qt::Key_S, RoomConnectionMask::SOUTH}, {Qt::Key_Down, RoomConnectionMask::SOUTH},
		{Qt::Key_D, RoomConnectionMask::WEST},  {Qt::Key_Right, RoomConnectionMask::WEST}
	};

	// Find pressed movement key
	RoomConnectionMask moveDirection = RoomConnectionMask::NONE;
	for (const auto& [key, direction] : keyToDirection) {
		if (gameContext.keyboardKeys[key]) {
			moveDirection = direction;
			break;
		}
	}

	if (moveDirection == RoomConnectionMask::NONE || !gameContext.currentRoom ||
		!hasConnection(gameContext.currentRoom->connectionMask, moveDirection)) {
		return;
	}

	glm::ivec2 targetGrid = gameContext.currentRoom->gridPosition + directionOffsets.at(moveDirection);

	/*std::cout << "move data: " <<
		static_cast<uint32_t>(gameContext.currentRoom->connectionMask) << " " <<
		glm::to_string(directionOffsets.at(moveDirection)) << "\n";*/

	// Search for target room in dungeon
	for (DungeonRoom& room : gameContext.dungeonFloor.dungeonRooms) {
		if (room.gridPosition == targetGrid) {

			gameContext.targetRoom = &room;
			//std::cout << "current room position: " << glm::to_string(gameContext.currentRoom->position) << "\n";
			//std::cout << "new room position: " << glm::to_string(room.position) << "\n";
			gameContext.cameraStartPosition = camera.getPosition();
			gameContext.cameraTargetPosition = room.cameraPosition;
			gameContext.cameraCurrentTransitionTime = 0.0f;
			gameContext.isCameraTransitioning = true;

			gameContext.isRoomMovementHandled = true;
			break;
		}
	}
}

void Character::handleCameraTransition(double deltaTime)
{
	gameContext.cameraCurrentTransitionTime += deltaTime;

	float rawT = std::min(gameContext.cameraCurrentTransitionTime / gameContext.cameraTransitionDuration, 1.0f);
	float t = std::min(rawT * rawT * (3.0f - 2.0f * rawT), 1.0f); // smoothstep easing
	glm::vec3 newPos = glm::mix(gameContext.cameraStartPosition, gameContext.cameraTargetPosition, t);
	camera.setPosition(newPos);

	if (t >= 1.0f) {
		Dungeon::updateRoomsState(
			*gameContext.targetRoom,
			*gameContext.currentRoom,
			gameContext.dungeonFloor.dungeonRooms, 2
		);

		gameContext.currentRoom = gameContext.targetRoom;
		gameContext.targetRoom = nullptr;
		gameContext.isCameraTransitioning = false;
	}
}

DungeonRoom* Character::enterDungeonFloor(DungeonFloor& dungeonFloor)
{
	if (dungeonFloor.entrance != nullptr) {
		Dungeon::updateRoomsState(
			*dungeonFloor.entrance, *dungeonFloor.entrance, dungeonFloor.dungeonRooms, 2
		);

		this->camera.setPosition(dungeonFloor.entrance->cameraPosition);

		return dungeonFloor.entrance;
	}
	else {
		std::cout << "there is no entrance room for this dungeon floor\n";
		return nullptr;
	}
}

void Character::handleCharacterMovement(
	float deltaTime, 
	float gravity, const std::vector<Model>& models
)
{
	glm::vec3 verticalDisplacement(0.0f);
	glm::vec3 horizontalDisplacement(0.0f);
	float movementSpeed = moveSpeed * deltaTime;
	
	glm::vec3 verticalWorldAxis = camera.getVerticalWorldAxis();
	glm::vec3 cameraDirection = camera.getDirection();
	glm::vec3 cameraPosition = camera.getPosition();
	glm::vec3 rightVector = glm::normalize(glm::cross(cameraDirection, verticalWorldAxis));

	if (gameContext.keyboardKeys[Qt::Key_Control]) {
		movementSpeed *= 1.9;
	}
	if (gameContext.keyboardKeys[Qt::Key_Alt]) {
		movementSpeed *= 0.4;
	}
	if (gameContext.keyboardKeys[Qt::Key_W]) {
		//std::cout << "w pressed\n";
		horizontalDisplacement += movementSpeed * glm::normalize(
			glm::vec3(cameraDirection.x, 0.0f, cameraDirection.z));
	}
	if (gameContext.keyboardKeys[Qt::Key_A]) {
		//std::cout << "a pressed\n";
		horizontalDisplacement -= rightVector * movementSpeed;
	}
	if (gameContext.keyboardKeys[Qt::Key_S]) {
		//std::cout << "s pressed\n";
		horizontalDisplacement -= movementSpeed * glm::normalize(
			glm::vec3(cameraDirection.x, 0.0f, cameraDirection.z));
	}
	if (gameContext.keyboardKeys[Qt::Key_D]) {
		//std::cout << "d pressed\n";
		horizontalDisplacement += rightVector * movementSpeed;
	}
	if (gameContext.keyboardKeys[Qt::Key_Space]) {
		if (isOnGround && gamemode == Gamemode::SURVIVAL) {
			velocity.y = jumpSpeed;
			isOnGround = false;
		}
		if (gamemode == Gamemode::CREATIVE) {
			verticalDisplacement += verticalWorldAxis * movementSpeed * 0.7f;
		}
	}
	if (gameContext.keyboardKeys[Qt::Key_Shift]) {
		if (gamemode == Gamemode::CREATIVE) {
			verticalDisplacement -= verticalWorldAxis * movementSpeed * 0.8f;
		}
	}

	if (!isOnGround && gamemode == Gamemode::SURVIVAL) {
		velocity.y -= gravity * deltaTime;
		verticalDisplacement = glm::vec3(0.0f, velocity.y * deltaTime, 0.0f);
	}

	glm::vec3 surfaceNormal;
	// horizontal check
	if (CollisionManager::checkCollisionWithCuboid(models, aabb + cameraPosition + horizontalDisplacement, surfaceNormal)) {
		float slopeAngle = glm::degrees(glm::acos(glm::dot(surfaceNormal, verticalWorldAxis)));

		if (slopeAngle <= maxSlopeAngle) {
			// Adjust movement along the slope
			glm::vec3 slopeAdjustment = glm::dot(horizontalDisplacement, surfaceNormal) * surfaceNormal;
			horizontalDisplacement -= slopeAdjustment;

			// slight vertical correction, to not get stuck in ground
			// cameraPosition.y += std::abs(slopeAdjustment.y * 0.01f);
		}
		else {
			// Too steep, restrict movement
			horizontalDisplacement = glm::vec3(0.0f);
			verticalDisplacement = glm::vec3(0.0f);
		}
		isOnGround = true;
	}
	else {
		isOnGround = false;
	}

	// vertical check
	if (CollisionManager::checkCollisionWithCuboid(models, aabb + cameraPosition + verticalDisplacement, surfaceNormal)) {
		velocity.y = 0.0f;
		isOnGround = true;
		verticalDisplacement = glm::vec3(0.0f);
	}
	else {
		isOnGround = false;
	}

	cameraPosition += verticalDisplacement;
	cameraPosition += horizontalDisplacement;

	camera.setPosition(cameraPosition);
}
