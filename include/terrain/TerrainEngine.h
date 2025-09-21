#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace TS {

struct TerrainVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 color;
};

class TerrainMesh {
private:
    std::vector<TerrainVertex> m_vertices;
    std::vector<unsigned int> m_indices;
    unsigned int m_VAO, m_VBO, m_EBO;
    int m_width, m_height;
    
public:
    TerrainMesh();
    ~TerrainMesh();
    
    void GenerateFromHeightmap(const std::vector<float>& heightData, int width, int height, float scale = 1.0f);
    void Render();
    void SetupMesh();
    float GetHeightAt(float x, float z) const;
};

class TerrainEngine {
private:
    std::unique_ptr<TerrainMesh> m_terrainMesh;
    std::vector<float> m_heightData;
    int m_width, m_height;
    float m_minHeight, m_maxHeight;
    float m_terrainScale;
    
    void RenderContourLines() const;
    
public:
    TerrainEngine();
    ~TerrainEngine();
    
    bool LoadTerrain(const std::string& filepath);
    void GenerateRandomTerrain(int width = 512, int height = 512);
    void Render();
    
    float GetElevationAt(float x, float z) const;
    bool HasLineOfSight(const glm::vec3& from, const glm::vec3& to) const;
    glm::vec3 GetTerrainSize() const;
    
    bool IsLoaded() const { return m_terrainMesh != nullptr && !m_heightData.empty(); }
};

}
