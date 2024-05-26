#include "pch.h"
#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(size_t seed) : generator(seed) {}

std::vector<std::vector<float>> TerrainGenerator::generateTerrain(size_t size, float roughness) {
    // Initialize the heightmap with zeros
    std::vector<std::vector<float>> heightmap(size, std::vector<float>(size, 0.0f));

    // Set the corner heights randomly
    heightmap[0][0] = getRandomHeight();
    heightmap[0][size - 1] = getRandomHeight();
    heightmap[size - 1][0] = getRandomHeight();
    heightmap[size - 1][size - 1] = getRandomHeight();

    // Perform the Diamond-Square algorithm
    diamondSquare(heightmap, 0, 0, size - 1, size - 1, roughness);

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
    heightmap[x1][my] = (heightmap[x1][y1] + heightmap[x1][y2] + heightmap[mx][my]) / 3.0f + getRandomOffset(roughness);
    heightmap[mx][y1] = (heightmap[x1][y1] + heightmap[x2][y1] + heightmap[mx][my]) / 3.0f + getRandomOffset(roughness);
    heightmap[x2][my] = (heightmap[x2][y1] + heightmap[x2][y2] + heightmap[mx][my]) / 3.0f + getRandomOffset(roughness);
    heightmap[mx][y2] = (heightmap[x1][y2] + heightmap[x2][y2] + heightmap[mx][my]) / 3.0f + getRandomOffset(roughness);

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
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < length; j++) {
            float x = static_cast<float>(i) * scale;
            float y = heightmap[i][j] * scale;
            float z = static_cast<float>(j) * scale;

            // Create vertex
            Vertex vertex{};
            vertex.pos = glm::vec3(x, y, z);
            vertex.color = glm::vec3(0.5f, 0.5f, 0.5f);
            vertex.texCoord0 = glm::vec2(static_cast<float>(i) / width, static_cast<float>(j) / length);

            model.vertices.push_back(vertex);
        }
    }

    // Generate indices
    for (size_t i = 0; i < width - 1; ++i) {
        for (size_t j = 0; j < length - 1; ++j) {
            // Calculate indices for each quad
            size_t topLeft = static_cast<size_t>(i * length + j);
            size_t topRight = static_cast<size_t>(topLeft + 1);
            size_t bottomLeft = static_cast<size_t>((i + 1) * length + j);
            size_t bottomRight = static_cast<size_t>(bottomLeft + 1);

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
        size_t idx0 = model.indices[i];
        size_t idx1 = model.indices[i + 1];
        size_t idx2 = model.indices[i + 2];

        // Calculate triangle normal
        glm::vec3 edge1 = model.vertices[idx1].pos - model.vertices[idx0].pos;
        glm::vec3 edge2 = model.vertices[idx2].pos - model.vertices[idx0].pos;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // Assign the same normal to all three vertices of the triangle
        model.vertices[idx0].normal += normal;
        model.vertices[idx1].normal += normal;
        model.vertices[idx2].normal += normal;
    }
}
