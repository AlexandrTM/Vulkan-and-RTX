#include "pch.h"
#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator(size_t seed) : generator(seed) 
{
    // Initialize the permutation vector with values 0-255
    permutation.resize(256);
    std::iota(permutation.begin(), permutation.end(), 0);

    // Shuffle the permutation vector
    std::shuffle(permutation.begin(), permutation.end(), std::default_random_engine(seed));

    // Duplicate the permutation vector
    permutation.insert(permutation.end(), permutation.begin(), permutation.end());
}

std::vector<std::vector<float>> TerrainGenerator::generateDiamondHeightMap(
    size_t width, size_t length, float roughness) {
    // Initialize the heightmap with zeros
    std::vector<std::vector<float>> heightmap(width, std::vector<float>(length, 0.0f));

    // Set the corner heights randomly
    heightmap[0        ][0         ] = roughness == 0.0 ? 0.0 : getRandomHeight();
    heightmap[0        ][length - 1] = roughness == 0.0 ? 0.0 : getRandomHeight();
    heightmap[width - 1][0         ] = roughness == 0.0 ? 0.0 : getRandomHeight();
    heightmap[width - 1][length - 1] = roughness == 0.0 ? 0.0 : getRandomHeight();

    // Perform the Diamond-Square algorithm
    diamondSquare(heightmap, 0, 0, width - 1, length - 1, roughness);

    return heightmap;
}

std::vector<std::vector<float>> TerrainGenerator::generatePerlinHeightMap(size_t width, size_t length, 
    float scale, float height) {
    std::vector<std::vector<float>> heightmap(width, std::vector<float>(length));
    
    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < length; ++j) {
            float x = static_cast<float>(i) * scale;
            float y = static_cast<float>(j) * scale;
            heightmap[i][j] = perlinNoise(x, y) * height;
        }
    }

    return heightmap;
}

// Generate a pseudo-random gradient vector at each grid point
float TerrainGenerator::grad(size_t hash, float x, float y) {
    // Convert low 4 bits of hash code into 12 gradient directions
    size_t h = hash & 15;   // Take the low 4 bits of the hash value
    float u = h < 8 ? x : y;  // Use x or y based on the value of h
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0);  // Determine v based on h
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);  // Calculate dot product
}

float TerrainGenerator::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// linear interpolation between a and b with weight t
float TerrainGenerator::lerp(float a, float b, float t) {
    return a + t * (b - a);
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

float TerrainGenerator::perlinNoise(float x, float y, float z) {
    // Find unit grid cell containing point
    size_t X = static_cast<int>(std::floor(x)) & 255;
    size_t Y = static_cast<int>(std::floor(y)) & 255;
    size_t Z = static_cast<int>(std::floor(z)) & 255;

    // Find fractional part of x, y, z in grid cell
    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    // Compute fade curves for x, y, z
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    // Hash coordinates of the 4 square corners
    size_t a = (permutation[ X      & 255] + Y) & 255;
    size_t b = (permutation[(X + 1) & 255] + Y) & 255;

    size_t aa = permutation[permutation[a    ] + Z];
    size_t ab = permutation[permutation[a + 1] + Z];
    size_t ba = permutation[permutation[b    ] + Z];
    size_t bb = permutation[permutation[b + 1] + Z];

    const float p0 = grad(aa, x    , y    );
    const float p1 = grad(ba, x - 1, y    );
    const float p2 = grad(ab, x    , y - 1);
    const float p3 = grad(bb, x - 1, y - 1);
    const float p4 = grad(permutation[(aa + 1) & 255], x    , y    );
    const float p5 = grad(permutation[(ba + 1) & 255], x - 1, y    );
    const float p6 = grad(permutation[(ab + 1) & 255], x    , y - 1);
    const float p7 = grad(permutation[(bb + 1) & 255], x - 1, y - 1);

    float q0 = lerp(p0, p1, u);
    float q1 = lerp(p2, p3, u);
    float q2 = lerp(p4, p5, u);
    float q3 = lerp(p6, p7, u);
        
    float r0 = lerp(q0, q1, v);
    float r1 = lerp(q2, q3, v);

    // And add blended results from 4 corners of the square
    float res = lerp(r0, r1, w);
    /*if (res > 0.85 || res < -0.85) {
        std::cout << res << "\n";
    }*/
    return res; // result is in range [-1, 1]
    //return (res + 1) / 2; // result is in range [0, 1]
}

void TerrainGenerator::generateTerrainMesh(float startX, float startZ,
    const std::vector<std::vector<float>>& heightmap, float gridSize, Mesh& mesh) {
    // Calculate terrain dimensions
    size_t width = heightmap.size();
    size_t length = heightmap[0].size();

    // Generate vertices
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < length; j++) {
            float x0 = static_cast<float>(i)        * gridSize + startX;
            float y0 =          heightmap[i][j]     * gridSize;
            float z0 = static_cast<float>   (j)     * gridSize + startZ;

            float x1 = static_cast<float>(i + 1)    * gridSize + startX;
            float y1 =          heightmap[i == width - 1 ? width - 1 : i + 1][j] * gridSize;
            float z1 = static_cast<float>   (j)     * gridSize + startZ;

            float x2 = static_cast<float>(i)        * gridSize + startX;
            float y2 =          heightmap[i][j == length - 1 ? length - 1 : j + 1] * gridSize;
            float z2 = static_cast<float>   (j + 1) * gridSize + startZ;

            float x3 = static_cast<float>(i + 1)    * gridSize + startX;
            float y3 =          heightmap[i == width - 1 ? width - 1 : i + 1]
                                            [j == length - 1 ? length - 1 : j + 1] * gridSize;
            float z3 = static_cast<float>   (j + 1) * gridSize + startZ;

            // Create vertex
            Vertex v0{}, v1{}, v2{}, v3{};
            v0.position = glm::vec3(x0, y0, z0);
            v1.position = glm::vec3(x1, y1, z1);
            v2.position = glm::vec3(x2, y2, z2);
            v3.position = glm::vec3(x3, y3, z3);

            v0.color = glm::vec3(0.5f, 0.5f, 0.5f);
            v1.color = glm::vec3(0.5f, 0.5f, 0.5f);
            v2.color = glm::vec3(0.5f, 0.5f, 0.5f);
            v3.color = glm::vec3(0.5f, 0.5f, 0.5f);

            /*v0.texCoord0 = glm::vec2(0.0f, 0.0f);
            v1.texCoord0 = glm::vec2(1.0f, 0.0f);
            v2.texCoord0 = glm::vec2(0.0f, 1.0f);
            v3.texCoord0 = glm::vec2(1.0f, 1.0f);*/

            float metricTextureSize = 10.0f;

            // relative
            float rx0 = x0 / metricTextureSize;
            float rx1 = x1 / metricTextureSize;
            float rx2 = x2 / metricTextureSize;
            float rx3 = x3 / metricTextureSize;
            float rz0 = z0 / metricTextureSize;
            float rz1 = z1 / metricTextureSize;
            float rz2 = z2 / metricTextureSize;
            float rz3 = z3 / metricTextureSize;

            // integer part
            /*float irx0 = std::floor(rx0);
            float irx1 = std::floor(rx1);
            float irx2 = std::floor(rx2);
            float irx3 = std::floor(rx3);
            float irz0 = std::floor(rz0);
            float irz1 = std::floor(rz1);
            float irz2 = std::floor(rz2);
            float irz3 = std::floor(rz3);*/

            v0.texCoord0 = glm::vec2(rx0, rz0);
            v1.texCoord0 = glm::vec2(rx1, rz1);
            v2.texCoord0 = glm::vec2(rx2, rz2);
            v3.texCoord0 = glm::vec2(rx3, rz3);

            //std::cout << v0.texCoord0[0] << " " << v0.texCoord0[1] << "\n";

            mesh.vertices.push_back(v0);
            mesh.vertices.push_back(v1);
            mesh.vertices.push_back(v2);
            mesh.vertices.push_back(v3);
        }
    }
    std::cout << "terrain vertices: " << mesh.vertices.size() << "\n";

    // Generate indices
    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < length; ++j) {
            // Calculate indices for each quad
            size_t topLeft     = static_cast<size_t>(i * (length) + j) * 4;
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
                mesh.indices.push_back(localIndices[k]);
            }
        }
    }

    std::cout << "terrain indices: " << mesh.indices.size() << "\n";

    // Calculate normals
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        size_t idx0 = mesh.indices[i    ];
        size_t idx1 = mesh.indices[i + 1];
        size_t idx2 = mesh.indices[i + 2];

        // Calculate triangle normal
        glm::vec3 edge1 = mesh.vertices[idx1].position - mesh.vertices[idx0].position;
        glm::vec3 edge2 = mesh.vertices[idx2].position - mesh.vertices[idx0].position;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // Assign the same normal to all three vertices of the triangle
        mesh.vertices[idx0].normal = normal;
        mesh.vertices[idx1].normal = normal;
        mesh.vertices[idx2].normal = normal;
    }
}
