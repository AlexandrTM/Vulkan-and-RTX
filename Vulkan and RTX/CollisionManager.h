#pragma once

#include "Model.h"

namespace CollisionManager
{
	bool checkCollisionWithCuboid(
		const Mesh& mesh,
		const Cuboid& cuboid,
		glm::vec3& surfaceNormal
	);
	bool checkCollisionWithCuboid(
		const Model& model,
		const Cuboid& cuboid,
		glm::vec3& surfaceNormal
	);
	bool checkCollisionWithCuboid(
		const std::vector<Model>& models,
		const Cuboid& cuboid,
		glm::vec3& surfaceNormal
	);

	bool isAABBOverlap(
		const glm::vec3& min1, const glm::vec3& max1,
		const glm::vec3& min2, const glm::vec3& max2
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
}

