#include "pch.h"
#include "Character.h"

void Character::handleInGamePlayerInput(GameContext& gameContext)
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
void Character::handleDungeonExplorationPlayerInput(GameContext& gameContext)
{
	if (gameContext.keyboardKeys[Qt::Key_Escape]) {
		gameContext.requestedGameState = GameState::PAUSED;
	}

	handleDungeonRoomMovement(gameContext);
}

void Character::handleDungeonRoomMovement(GameContext& gameContext) 
{
	if (!gameContext.keyboardKeys[Qt::Key_W] &&
		!gameContext.keyboardKeys[Qt::Key_A] &&
		!gameContext.keyboardKeys[Qt::Key_S] &&
		!gameContext.keyboardKeys[Qt::Key_D]) {
		gameContext.roomMovementHandled = false;
	}

	// ensure per key release movement	// ensure no pending movement, to not skip dungeon room
	if (gameContext.roomMovementHandled || gameContext.requestedGameState != GameState::NONE) {
		return;
	}

	glm::ivec2 moveDirectionVector = glm::ivec2(0, 0);
	RoomConnectionMask moveDirection = RoomConnectionMask::NONE;
	if (gameContext.keyboardKeys[Qt::Key_W]) {
		//std::cout << "w pressed\n";
		moveDirection = RoomConnectionMask::SOUTH; // inverted
		moveDirectionVector = directionOffsets.at(RoomConnectionMask::SOUTH);
	}
	if (gameContext.keyboardKeys[Qt::Key_A]) {
		//std::cout << "a pressed\n";
		moveDirection = RoomConnectionMask::WEST;
		moveDirectionVector = directionOffsets.at(RoomConnectionMask::WEST);
	}
	if (gameContext.keyboardKeys[Qt::Key_S]) {
		//std::cout << "s pressed\n";
		moveDirection = RoomConnectionMask::NORTH;
		moveDirectionVector = directionOffsets.at(RoomConnectionMask::NORTH);
	}
	if (gameContext.keyboardKeys[Qt::Key_D]) {
		//std::cout << "d pressed\n";
		moveDirection = RoomConnectionMask::EAST;
		moveDirectionVector = directionOffsets.at(RoomConnectionMask::EAST);
	}

	if (moveDirectionVector != glm::ivec2(0, 0) and gameContext.currentRoom != nullptr) {
		if (hasConnection(gameContext.currentRoom->connectionMask, moveDirection)) {
			/*std::cout << "move data: " <<
				static_cast<uint32_t>(gameContext.currentRoom->connectionMask) << " " <<
				glm::to_string(moveDirectionVector) << "\n";*/
			glm::ivec2 targetGrid = gameContext.currentRoom->gridPosition + moveDirectionVector;

			// Search for target room in dungeon
			for (DungeonRoom& room : gameContext.dungeonFloor.dungeonRooms) {
				if (room.gridPosition == targetGrid) {
					gameContext.currentRoom = &room;
					camera.setPosition(room.cameraPosition);
					gameContext.roomMovementHandled = true;
					break;
				}
			}
		}
	}
}

void Character::handleCharacterMovement(
	GameContext& gameContext,
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
