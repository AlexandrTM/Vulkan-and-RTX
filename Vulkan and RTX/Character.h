#include "pch.h"
#include "Camera.h"
#include "InteractableVolume.h"
#include "Model.h"

#ifndef CHARACTER_H
#define CHARACTER_H

class Character
{
private:
	double sensitivity = 0.125;

	bool keys[1024] = { 0 };

	glm::vec3 velocity = glm::vec3(0.0f);
	bool isOnGround = false;

	glm::vec3 bound0 = glm::vec3(-0.3, -1.45, -0.3);
	glm::vec3 bound1 = glm::vec3(0.3, 0.25, 0.3);

public:
	Camera camera;

	std::vector<InteractableVolume> interactableCuboids = { 
		InteractableVolume(glm::vec3(20.0, 0.0, -10.0), glm::vec3(21.75, 4.75, -8.25), "easy"),
		InteractableVolume(glm::vec3(20.0, 0.0,   0.0), glm::vec3(21.75, 4.75,  1.75), "medium"),
		InteractableVolume(glm::vec3(20.0, 0.0,  10.0), glm::vec3(21.75, 4.75, 11.75), "hard")
	};

	InteractableVolume* currentInteractingVolume = nullptr;

	void movePerson(
		float deltaTime, float moveSpeed, float jumpSpeed,
		float gravity, Mesh& mesh
	);
	bool checkCollisionWithMesh(
		const Mesh& mesh,
		const glm::vec3& cameraPosition,
		glm::vec3& surfaceNormal
	);
	bool triangleBoxIntersection(
		const glm::vec3& boxMin, const glm::vec3& boxMax,
		const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
		const glm::vec3& triangleNormal
	);
	bool isOverlapOnAxis(
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