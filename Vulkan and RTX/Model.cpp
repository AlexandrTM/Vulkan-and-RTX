#include "pch.h"
#include "Model.h"

void computeAABB(Mesh& mesh) {
    // Initialize min and max to extremes
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(std::numeric_limits<float>::lowest());

    for (const auto& vertex : mesh.vertices) {
        min = glm::min(min, vertex.position);
        max = glm::max(max, vertex.position);
    }

    mesh.aabb.min = min;
    mesh.aabb.max = max;
}
