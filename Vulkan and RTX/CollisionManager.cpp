#include "pch.h"
#include "CollisionManager.h"

glm::vec3 cuboidMin{};
glm::vec3 cuboidMax{};

glm::vec3 meshBoxMin{};
glm::vec3 meshBoxMax{};

bool CollisionManager::checkCollisionWithCuboid(
	const Mesh& mesh,
	const Cuboid& cuboid,
	glm::vec3& surfaceNormal
)
{
	cuboidMin = cuboid.min;
	cuboidMax = cuboid.max;

	meshBoxMin = mesh.aabb.min;
	meshBoxMax = mesh.aabb.max;

	/*std::cout <<
		"character box: " <<
		glm::to_string(cameraPosition + aabb.min) << " " <<
		glm::to_string(cameraPosition + aabb.max) << "\n";*/

		// Cuboid characterAABB = { mesh.aabb.min, mesh.aabb.max };
		// Cuboid meshAABB = { cameraPosition + aabb.min, cameraPosition + aabb.max };

	if (isAABBOverlap(cuboidMin, cuboidMax, meshBoxMin, meshBoxMax)) {
		for (size_t i = 0; i < mesh.indices.size(); i += 3) {
			const glm::vec3& v0 = mesh.vertices[mesh.indices[i]].position;
			const glm::vec3& v1 = mesh.vertices[mesh.indices[i + 1]].position;
			const glm::vec3& v2 = mesh.vertices[mesh.indices[i + 2]].position;
			const glm::vec3& triangleNormal = mesh.vertices[mesh.indices[i]].normal;
			surfaceNormal = triangleNormal;

			if (triangleBoxIntersection(cuboidMin, cuboidMax, v0, v1, v2, triangleNormal)) {
				return true;
			}
		}
	}
	return false;
}
bool CollisionManager::checkCollisionWithCuboid(
	const Model& model,
	const Cuboid& cuboid,
	glm::vec3& surfaceNormal
)
{
	if (model.isCollidable) {
		for (const Mesh& mesh : model.meshes) {
			if (checkCollisionWithCuboid(mesh, cuboid, surfaceNormal)) {
				return true;
			}
		}
	}
	return false;
}
bool CollisionManager::checkCollisionWithCuboid(
	const std::vector<Model>& models,
	const Cuboid& cuboid,
	glm::vec3& surfaceNormal
)
{
	for (const Model& model : models) {
		if (checkCollisionWithCuboid(model, cuboid, surfaceNormal)) {
			return true;
		}
	}
	return false;
}

bool CollisionManager::isAABBOverlap(
	const glm::vec3& min1, const glm::vec3& max1,
	const glm::vec3& min2, const glm::vec3& max2
) {
	return
		(min1.x <= max2.x && max1.x >= min2.x) &&
		(min1.y <= max2.y && max1.y >= min2.y) &&
		(min1.z <= max2.z && max1.z >= min2.z);
}

static constexpr glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
static constexpr glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
static constexpr glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

bool CollisionManager::triangleBoxIntersection(const glm::vec3& boxMin, const glm::vec3& boxMax,
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

bool CollisionManager::isOverlapOnAxis(
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