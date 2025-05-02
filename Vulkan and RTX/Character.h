#include "pch.h"

#ifndef CHARACTER_H
#define CHARACTER_H

#include "Camera.h"
#include "CollisionManager.h"
#include "InteractableVolume.h"
#include "Model.h"
#include "gamecontext_instance.h"
#include "DungeonComponents.h"
#include "Mob.h"

class Character
{
private:
	Gamemode gamemode = Gamemode::CREATIVE;
	//float moveSpeed = 2.1f;
	float moveSpeed = 4.0f;
	float jumpSpeed = 4.0f;
	float maxSlopeAngle = 50.0f;

	glm::vec3 velocity = glm::vec3(0.0f);
	bool isOnGround = false;

	Cuboid aabb = { glm::vec3(-0.3, -1.45, -0.3), glm::vec3(0.3, 0.25, 0.3) };

public:
	Camera camera;
	double mouseSensitivity = 0.125;

	int32_t health = 30;
	int32_t maxHealth = 30;
	int32_t attackPower = 4;
	int32_t defence = 0;
	int32_t experience = 0;

	bool isAlive() const {
		return health > 0;
	}

	void takeDamage(const std::vector<Mob>& mobs) {
		for (const Mob& mob : mobs) { takeDamage(mob); }
	}
	void takeDamage(const Mob& mob) {
		takeDamage(mob.attackPower);
	}
	void takeDamage(int32_t damage) {
		int32_t damageTaken = std::max(damage - defence, static_cast<int32_t>(0));
		health = std::max(health - damageTaken, 0);
	}

	void handleCharacterMovement(
		float deltaTime,
		float gravity, const std::vector<Model>& models
	);
	void handleInGamePlayerInput();
	void handleDungeonExplorationPlayerInput();
	void handleDungeonRoomMovement();

	// ========== not used currently ==========
	std::vector<InteractableVolume> interactableCuboids = {
		InteractableVolume(glm::vec3(20.0, 0.0, -10.0), glm::vec3(21.75, 4.75, -8.25), "easy"),
		InteractableVolume(glm::vec3(20.0, 0.0,   0.0), glm::vec3(21.75, 4.75,  1.75), "medium"),
		InteractableVolume(glm::vec3(20.0, 0.0,  10.0), glm::vec3(21.75, 4.75, 11.75), "hard")
	};

	InteractableVolume* isInteracting = nullptr;
	// ========================================
};

#endif // !CHARACTER_H