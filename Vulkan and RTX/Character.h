#include "pch.h"
#include "Camera.h"
#include "InteractableVolume.h"
#include "Model.h"

#ifndef CHARACTER_H
#define CHARACTER_H

typedef enum Gamemode {
	GAMEMODE_CREATIVE = 0,
	GAMEMODE_SURVIVAL = 1
} Gamemode;

class Character
{
private:
	double sensitivity = 0.125;
	bool keys[1024] = { 0 };

	Cuboid aabb = { glm::vec3(-0.3, -1.45, -0.3), glm::vec3(0.3, 0.25, 0.3) };
	float maxSlopeAngle = 50.0f;

	glm::vec3 velocity = glm::vec3(0.0f);
	bool isOnGround = false;

	Gamemode gamemode = GAMEMODE_CREATIVE;
	float moveSpeed = 2.1f;
	float jumpSpeed = 4.0f;

public:
	Camera camera;

	std::vector<InteractableVolume> interactableCuboids = { 
		InteractableVolume(glm::vec3(20.0, 0.0, -10.0), glm::vec3(21.75, 4.75, -8.25), "easy"),
		InteractableVolume(glm::vec3(20.0, 0.0,   0.0), glm::vec3(21.75, 4.75,  1.75), "medium"),
		InteractableVolume(glm::vec3(20.0, 0.0,  10.0), glm::vec3(21.75, 4.75, 11.75), "hard")
	};

	InteractableVolume* currentInteractingVolume = nullptr;

	void handleKeyInput(
		float deltaTime,
		float gravity, const std::vector<Model>& models
	);
	bool checkCollisionWithMesh(
		const Mesh& mesh,
		const glm::vec3& cameraPosition,
		glm::vec3& surfaceNormal
	) const;
	bool checkCollisionWithModel(
		const Model& model,
		const glm::vec3& cameraPosition,
		glm::vec3& surfaceNormal
	) const;
	bool checkCollisionWithModels(
		const std::vector<Model>& models,
		const glm::vec3& cameraPosition,
		glm::vec3& surfaceNormal
	) const;

	static bool isAABBOverlap(
		const glm::vec3& min1, const glm::vec3& max1,
		const glm::vec3& min2, const glm::vec3& max2
	);
	static bool triangleBoxIntersection(
		const glm::vec3& boxMin, const glm::vec3& boxMax,
		const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
		const glm::vec3& triangleNormal
	);
	static bool isOverlapOnAxis(
		const glm::vec3& axis, const glm::vec3& boxHalfSize,
		const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2
	);

	void initializeInputHandler(GLFWwindow* window);

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	// mouse wheel handling
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif // !CHARACTER_H