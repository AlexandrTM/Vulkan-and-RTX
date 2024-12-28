#include "pch.h"
#include "Model.h"

#ifndef TERRAIN_GENERATOR
#define TERRAIN_GENERATOR

class TerrainGenerator
{
public:
    TerrainGenerator(size_t seed);

    std::vector<std::vector<float>> generateDiamondHeightMap(
        size_t width, size_t length, 
        float roughness
    );
    std::vector<std::vector<float>> generatePerlinHeightMap(
        size_t chunkXoffset, size_t chunkZoffset,
        size_t chunkWidth, size_t chunkLength,
        float scale, float height
    );

    static void generateTerrain(
        float startX, float startY, float startZ,
        size_t chunkWidth, size_t chunkLength,
        size_t chunkRows, size_t chunkCols,
        float gridSize, float scale, float height,
        size_t seed, std::vector<Model>& models,
        Texture& texture,
        TerrainGenerator* terrainGenerator
    );

    void generateTerrainMesh(
        float offsetX, float offsetY, float offsetZ,
        const std::vector<std::vector<float>>& heightmap,
        float gridSize,
        Mesh& mesh
    );

private:
    std::vector<uint32_t> permutation;
    std::default_random_engine generator;

    // Perform the Diamond-Square algorithm recursively
    void diamondSquare(
        std::vector<std::vector<float>>& heightmap,
        size_t x1, size_t y1, size_t x2, size_t y2, 
        float roughness
    );

    // Get a random height offset based on the roughness
    float getRandomOffset(float roughness);
    float getRandomNormalizedReal();

    float perlinNoise(float x, float y, float z = 0.34567);
    float fade(float t);
    float lerp(float t, float a, float b);
    float grad(size_t hash, float x, float y);
};

#endif // !TERRAIN_GENERATOR