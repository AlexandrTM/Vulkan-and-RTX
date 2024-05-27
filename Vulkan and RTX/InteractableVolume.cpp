#include "pch.h"
#include "InteractableVolume.h"

InteractableVolume::InteractableVolume(glm::vec3 x0, glm::vec3 x1, std::string name) : 
    bound0(x0), bound1(x1), name(name) {}

InteractableVolume::InteractableVolume() {}

bool InteractableVolume::rayIntersectsCuboid(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const
{
    glm::vec3 invDir = 1.0f / rayDirection;
    glm::vec3 t0s = (bound0 - rayOrigin) * invDir;
    glm::vec3 t1s = (bound1 - rayOrigin) * invDir;

    glm::vec3 tmins = glm::min(t0s, t1s);
    glm::vec3 tmaxs = glm::max(t0s, t1s);

    float tmin = glm::max(glm::max(tmins.x, tmins.y), tmins.z);
    float tmax = glm::min(glm::min(tmaxs.x, tmaxs.y), tmaxs.z);

    if (tmax < 0 || tmin > tmax) {
        return false;
    }

    return (tmin <= maxInteractionDistance);
}

