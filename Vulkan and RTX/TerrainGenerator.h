#include "pch.h"
#include "Model.h"

#ifndef TERRAIN_GENERATOR
#define TERRAIN_GENERATOR

class TerrainGenerator
{
public:
    TerrainGenerator(size_t seed);

    std::vector<std::vector<float>> generateDiamondHeightMap(size_t width, size_t height, float roughness);

    void generateTerrainMesh(float startX, float startZ,
        const std::vector<std::vector<float>>& heightmap, float scale, Model& model);

private:
    std::mt19937 generator;

    // Perform the Diamond-Square algorithm recursively
    void diamondSquare(std::vector<std::vector<float>>& heightmap,
        size_t x1, size_t y1, size_t x2, size_t y2, float roughness);

    // Get a random height offset based on the roughness
    float getRandomOffset(float roughness);

    // Get a random height value
    float getRandomHeight();
};

#endif // !TERRAIN_GENERATOR