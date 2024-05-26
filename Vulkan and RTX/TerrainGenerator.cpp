#include "pch.h"
#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(size_t seed) : generator(seed) {}

std::vector<std::vector<float>> TerrainGenerator::generateHeightMap(
    size_t width, size_t height, float roughness) {
    // Initialize the heightmap with zeros
    std::vector<std::vector<float>> heightmap(width, std::vector<float>(height, 0.0f));

    // Set the corner heights randomly
    heightmap[0        ][0         ] = roughness == 0.0 ? 0.0 : getRandomHeight();
    heightmap[0        ][height - 1] = roughness == 0.0 ? 0.0 : getRandomHeight();
    heightmap[width - 1][0         ] = roughness == 0.0 ? 0.0 : getRandomHeight();
    heightmap[width - 1][height - 1] = roughness == 0.0 ? 0.0 : getRandomHeight();

    // Perform the Diamond-Square algorithm
    diamondSquare(heightmap, 0, 0, width - 1, height - 1, roughness);

    return heightmap;
}

// Perform the Diamond-Square algorithm recursively
void TerrainGenerator::diamondSquare(std::vector<std::vector<float>>& heightmap, 
    size_t x1, size_t y1, size_t x2, size_t y2, float roughness) {
    if (x2 - x1 < 2 || y2 - y1 < 2) {
        return;
    }

    size_t mx = (x1 + x2) / 2;
    size_t my = (y1 + y2) / 2;

    // Diamond step
    float sum = heightmap[x1][y1] + heightmap[x2][y1] + heightmap[x1][y2] + heightmap[x2][y2];
    float avg = sum / 4.0f;
    heightmap[mx][my] = avg + getRandomOffset(roughness);

    // Square step
    heightmap[x1][my] = (heightmap[x1][y1] + heightmap[x1][y2] 
        + heightmap[mx][my]) / 3.0f + getRandomOffset(roughness);
    heightmap[mx][y1] = (heightmap[x1][y1] + heightmap[x2][y1] 
        + heightmap[mx][my]) / 3.0f + getRandomOffset(roughness);
    heightmap[x2][my] = (heightmap[x2][y1] + heightmap[x2][y2] 
        + heightmap[mx][my]) / 3.0f + getRandomOffset(roughness);
    heightmap[mx][y2] = (heightmap[x1][y2] + heightmap[x2][y2] 
        + heightmap[mx][my]) / 3.0f + getRandomOffset(roughness);

    // Recursively apply the algorithm to the four quadrants
    diamondSquare(heightmap, x1, y1, mx, my, roughness);
    diamondSquare(heightmap, mx, y1, x2, my, roughness);
    diamondSquare(heightmap, x1, my, mx, y2, roughness);
    diamondSquare(heightmap, mx, my, x2, y2, roughness);
}

// Get a random height offset based on the roughness
float TerrainGenerator::getRandomOffset(float roughness) {
    std::uniform_real_distribution<float> distribution(-roughness, roughness);
    return distribution(generator);
}

// Get a random height value
float TerrainGenerator::getRandomHeight() {
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(generator);
}

void TerrainGenerator::generateTerrainMesh(const std::vector<std::vector<float>>& heightmap, 
    float scale, Model& model) {
    // Calculate terrain dimensions
    size_t width = heightmap.size();
    size_t length = heightmap[0].size();

    // Generate vertices
    for (size_t i = 0; i < width - 1; i++) {
        for (size_t j = 0; j < length - 1; j++) {
            float x0 = static_cast<float>(i) * scale;
            float y0 = heightmap[i][j] * scale;
            float z0 = static_cast<float>(j) * scale;

            float x1 = static_cast<float>(i + 1) * scale;
            float y1 = heightmap[i + 1][j] * scale;
            float z1 = static_cast<float>(j) * scale;

            float x2 = static_cast<float>(i) * scale;
            float y2 = heightmap[i][j + 1] * scale;
            float z2 = static_cast<float>(j + 1) * scale;

            float x3 = static_cast<float>(i + 1) * scale;
            float y3 = heightmap[i + 1][j + 1] * scale;
            float z3 = static_cast<float>(j + 1) * scale;

            // Create vertex
            Vertex v0{}, v1{}, v2{}, v3{};
            v0.pos = glm::vec3(x0, y0, z0);
            v1.pos = glm::vec3(x1, y1, z1);
            v2.pos = glm::vec3(x2, y2, z2);
            v3.pos = glm::vec3(x3, y3, z3);

            v0.color = glm::vec3(0.5f, 0.5f, 0.5f);
            v1.color = glm::vec3(0.5f, 0.5f, 0.5f);
            v2.color = glm::vec3(0.5f, 0.5f, 0.5f);
            v3.color = glm::vec3(0.5f, 0.5f, 0.5f);

            v0.texCoord0 = glm::vec2(0.0f, 0.0f);
            v1.texCoord0 = glm::vec2(1.0f, 0.0f);
            v2.texCoord0 = glm::vec2(0.0f, 1.0f);
            v3.texCoord0 = glm::vec2(1.0f, 1.0f);

            model.vertices.push_back(v0);
            model.vertices.push_back(v1);
            model.vertices.push_back(v2);
            model.vertices.push_back(v3);
        }
    }

    // Generate indices
    for (size_t i = 0; i < width - 1; ++i) {
        for (size_t j = 0; j < length - 1; ++j) {
            // Calculate indices for each quad
            size_t topLeft     = static_cast<size_t>(i * (length - 1) + j) * 4;
            size_t topRight    = static_cast<size_t>(topLeft + 1);
            size_t bottomLeft  = static_cast<size_t>(topLeft + 2);
            size_t bottomRight = static_cast<size_t>(topLeft + 3);

            // Create triangles
            std::vector<size_t> localIndices =
            {
                topLeft, bottomLeft, topRight,
                topRight, bottomLeft, bottomRight
            };

            for (size_t k = 0; k < localIndices.size(); k++)
            {
                model.indices.push_back(localIndices[k]);
            }
        }
    }

    // Calculate normals
    for (size_t i = 0; i < model.indices.size(); i += 3) {
        size_t idx0 = model.indices[i    ];
        size_t idx1 = model.indices[i + 1];
        size_t idx2 = model.indices[i + 2];

        // Calculate triangle normal
        glm::vec3 edge1 = model.vertices[idx1].pos - model.vertices[idx0].pos;
        glm::vec3 edge2 = model.vertices[idx2].pos - model.vertices[idx0].pos;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // Assign the same normal to all three vertices of the triangle
        model.vertices[idx0].normal = normal;
        model.vertices[idx1].normal = normal;
        model.vertices[idx2].normal = normal;
    }
}
