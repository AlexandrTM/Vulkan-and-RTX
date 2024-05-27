#include "pch.h"

#ifndef INTERACTABLE_VOLUME
#define INTERACTABLE_VOLUME

struct InteractableVolume
{
	InteractableVolume(glm::vec3 bound0, glm::vec3 bound1);
	InteractableVolume();

	glm::vec3 bound0 = glm::vec3(0.0);
	glm::vec3 bound1 = glm::vec3(0.0);

	float maxInteractionDistance = 10.0f;

	bool rayIntersectsCuboid(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const;
};

#endif // !INTERACTABLE_VOLUME