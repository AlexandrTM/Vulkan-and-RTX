#include "pch.h"
#include "Character.h"
#include "Camera.h"

void Character::initializeInputHandler(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mode) {
		Character* character = static_cast<Character*>(glfwGetWindowUserPointer(window));
		character->keyCallback(window, key, scancode, action, mode);
		});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		Character* character = static_cast<Character*>(glfwGetWindowUserPointer(window));
		character->mouseButtonCallback(window, button, action, mods);
		});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		Character* character = static_cast<Character*>(glfwGetWindowUserPointer(window));
		character->mouseCallback(window, xpos, ypos);
		});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		Character* character = static_cast<Character*>(glfwGetWindowUserPointer(window));
		character->scrollCallback(window, xoffset, yoffset);
		});
}

void Character::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// altering mouse sensitivity
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		sensitivity = std::clamp(sensitivity * 1.3, 0.001, 10.0);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		sensitivity = std::clamp(sensitivity * 0.75, 0.001, 10.0);
	}

	if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
		if (currentInteractingVolume == nullptr) {
			for (size_t i = 0; i < interactableCuboids.size(); i++) {
				if (interactableCuboids[i].rayIntersectsCuboid(
					camera.getLookFrom(), camera.getDirection())) {
					currentInteractingVolume = &interactableCuboids[i];
					currentInteractingVolume->isOpen = true;
				}
			}
		}
	}

	// changing mipmap level of detail
	/*if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
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
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
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
void Character::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) 
	{
		camera.setVerticalFov(60.0f);
		sensitivity = 0.125;
	}
}
void Character::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (!currentInteractingVolume) {
		camera.rotate(xpos, ypos, sensitivity);
	}
}
// mouse wheel handling
void Character::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.setVerticalFov(std::clamp(camera.getVerticalFov() - static_cast<float>(yoffset), 0.1f, 130.0f));
}

void Character::handleKeyInput(
	float deltaTime, 
	float gravity, const std::vector<Model>& models
)
{
	glm::vec3 verticalDisplacement(0.0f);
	glm::vec3 horizontalDisplacement(0.0f);
	float movementSpeed = moveSpeed * deltaTime;
	
	glm::vec3 verticalWorldAxis = camera.getVerticalWorldAxis();
	glm::vec3 cameraDirection = camera.getDirection();
	glm::vec3 position = camera.getLookFrom();
	glm::vec3 rightVector = glm::normalize(glm::cross(cameraDirection, verticalWorldAxis));

	if (keys[GLFW_KEY_LEFT_CONTROL]) {
		movementSpeed *= 1.9;
	}
	if (keys[GLFW_KEY_LEFT_ALT]) {
		movementSpeed *= 0.4;
	}
	if (keys[GLFW_KEY_W]) {
		horizontalDisplacement += movementSpeed * glm::normalize(
			glm::vec3(cameraDirection.x, 0.0f, cameraDirection.z));
	}
	if (keys[GLFW_KEY_A]) {
		horizontalDisplacement -= rightVector * movementSpeed;
	}
	if (keys[GLFW_KEY_S]) {
		horizontalDisplacement -= movementSpeed * glm::normalize(
			glm::vec3(cameraDirection.x, 0.0f, cameraDirection.z));
	}
	if (keys[GLFW_KEY_D]) {
		horizontalDisplacement += rightVector * movementSpeed;
	}
	if (keys[GLFW_KEY_SPACE]) {
		if (gamemode == GAMEMODE_SURVIVAL && isOnGround) {
			velocity.y = jumpSpeed;
			isOnGround = false;
		}
		if (gamemode == GAMEMODE_CREATIVE) {
			verticalDisplacement += verticalWorldAxis * movementSpeed * 0.7f;
		}
	}
	if (keys[GLFW_KEY_LEFT_SHIFT]) {
		if (gamemode == GAMEMODE_CREATIVE) {
			verticalDisplacement -= verticalWorldAxis * movementSpeed * 0.8f;
		}
	}

	if (!isOnGround && gamemode == GAMEMODE_SURVIVAL) {
		velocity.y -= gravity * deltaTime;
		verticalDisplacement = glm::vec3(0.0f, velocity.y * deltaTime, 0.0f);
	}

	glm::vec3 surfaceNormal;
	// horizontal check
	if (checkCollisionWithModels(models, position + horizontalDisplacement, surfaceNormal)) {
		float slopeAngle = glm::degrees(glm::acos(glm::dot(surfaceNormal, verticalWorldAxis)));

		if (slopeAngle <= maxSlopeAngle) {
			// Adjust movement along the slope
			glm::vec3 slopeAdjustment = glm::dot(horizontalDisplacement, surfaceNormal) * surfaceNormal;
			horizontalDisplacement -= slopeAdjustment;

			// slight vertical correction, to not get stuck in ground
			// position.y += std::abs(slopeAdjustment.y * 0.01f);
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
	if (checkCollisionWithModels(models, position + verticalDisplacement, surfaceNormal)) {
		velocity.y = 0.0f;
		isOnGround = true;
		verticalDisplacement = glm::vec3(0.0f);
	}
	else {
		isOnGround = false;
	}

	position += verticalDisplacement;
	position += horizontalDisplacement;

	camera.setLookFrom(position);
}


bool Character::checkCollisionWithMesh(
	const Mesh& mesh, 
	const glm::vec3& cameraPosition, 
	glm::vec3& surfaceNormal
) const
{
	glm::vec3 characterBoxMin = cameraPosition + aabb.min;
	glm::vec3 characterBoxMax = cameraPosition + aabb.max;

	glm::vec3 meshBoxMin = mesh.aabb.min;
	glm::vec3 meshBoxMax = mesh.aabb.max;

	// Cuboid characterAABB = { mesh.aabb.min, mesh.aabb.max };
	// Cuboid meshAABB = { cameraPosition + aabb.min, cameraPosition + aabb.max };
	
	if (isAABBOverlap(characterBoxMin, characterBoxMax, meshBoxMin, meshBoxMax)) {
		for (size_t i = 0; i < mesh.indices.size(); i += 3) {
			const glm::vec3& v0 = mesh.vertices[mesh.indices[i]].position;
			const glm::vec3& v1 = mesh.vertices[mesh.indices[i + 1]].position;
			const glm::vec3& v2 = mesh.vertices[mesh.indices[i + 2]].position;
			const glm::vec3& triangleNormal = mesh.vertices[mesh.indices[i]].normal;
			surfaceNormal = triangleNormal;

			if (triangleBoxIntersection(characterBoxMin, characterBoxMax, v0, v1, v2, triangleNormal)) {
				return true;
			}
		}
	}
	return false;
}
bool Character::checkCollisionWithModel(
	const Model& model, 
	const glm::vec3& cameraPosition, 
	glm::vec3& surfaceNormal
) const
{
	for (size_t i = 0; i < model.meshes.size(); i++) {
		if (model.checkCollision && 
			checkCollisionWithMesh(model.meshes[i], cameraPosition, surfaceNormal)) {
			return true;
		}
	}
	return false;
}
bool Character::checkCollisionWithModels(
	const std::vector<Model>& models, 
	const glm::vec3& cameraPosition, 
	glm::vec3& surfaceNormal
) const
{
	for (size_t i = 0; i < models.size(); i++) {
		if (checkCollisionWithModel(models[i], cameraPosition, surfaceNormal)) {
			return true;
		}
	}
	return false;
}

bool Character::isAABBOverlap(
	const glm::vec3& min1, const glm::vec3& max1,
	const glm::vec3& min2, const glm::vec3& max2
) {
	return 
		(min1.x <= max2.x && max1.x >= min2.x) &&
		(min1.y <= max2.y && max1.y >= min2.y) &&
		(min1.z <= max2.z && max1.z >= min2.z);
}

static const glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
static const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
static const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

bool Character::triangleBoxIntersection(const glm::vec3& boxMin, const glm::vec3& boxMax,
	const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
	const glm::vec3& triangleNormal
) {
	// SAT-based Triangle-Cuboid intersection
	const glm::vec3 boxCenter = (boxMin + boxMax) * 0.5f;
	const glm::vec3 boxHalfSize = (boxMax - boxMin) * 0.5f;

	// Translate triangle vertices to the box's local space
	glm::vec3 tv0 = v0 - boxCenter;
	glm::vec3 tv1 = v1 - boxCenter;
	glm::vec3 tv2 = v2 - boxCenter;

	// Triangle edges
	glm::vec3 edge0 = tv1 - tv0;
	glm::vec3 edge1 = tv2 - tv1;
	glm::vec3 edge2 = tv0 - tv2;

	// Separating Axis Tests (SAT)
	glm::vec3 axes[10] = {
		// Triangle normal
		triangleNormal,

		// Cross products of edges with box axes
		glm::cross(edge0, xAxis),
		glm::cross(edge0, yAxis),
		glm::cross(edge0, zAxis),

		glm::cross(edge1, xAxis),
		glm::cross(edge1, yAxis),
		glm::cross(edge1, zAxis),

		glm::cross(edge2, xAxis),
		glm::cross(edge2, yAxis),
		glm::cross(edge2, zAxis),
	};

	for (const glm::vec3& axis : axes) {
		if (!isOverlapOnAxis(axis, boxHalfSize, tv0, tv1, tv2)) {
			return false; // Separating axis found
		}
	}
	return true; // No separating axis -> intersection
}

bool Character::isOverlapOnAxis(
	const glm::vec3& axis, const glm::vec3& boxHalfSize,
	const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2
) {
	// Project vertices onto axis
	float p0 = glm::dot(axis, v0);
	float p1 = glm::dot(axis, v1);
	float p2 = glm::dot(axis, v2);

	float minProj = glm::min(glm::min(p0, p1), p2);
	float maxProj = glm::max(glm::max(p0, p1), p2);

	// Project box onto axis
	float boxProj = 
		boxHalfSize.x * std::abs(axis.x) +
		boxHalfSize.y * std::abs(axis.y) +
		boxHalfSize.z * std::abs(axis.z);

	// Check for overlap
	return (minProj <= boxProj && maxProj >= -boxProj);
}
