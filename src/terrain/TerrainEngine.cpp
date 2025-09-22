#include "terrain/TerrainEngine.h"
#include <OpenGL/gl.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <ctime>
#include <unistd.h>  // for getpid()

namespace TS {

TerrainMesh::TerrainMesh() : m_VAO(0), m_VBO(0), m_EBO(0), m_width(0), m_height(0) {}

TerrainMesh::~TerrainMesh() {
    if (m_VAO) glDeleteVertexArraysAPPLE(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

void TerrainMesh::GenerateFromHeightmap(const std::vector<float>& heightData, int width, int height, float scale) {
    m_width = width;
    m_height = height;
    m_vertices.clear();
    m_indices.clear();
    
    // Generate vertices
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            TerrainVertex vertex;
            
            vertex.position.x = (float)x - width * 0.5f;
            vertex.position.y = heightData[z * width + x] * scale;
            vertex.position.z = (float)z - height * 0.5f;
            
            vertex.texCoord.x = (float)x / (width - 1);
            vertex.texCoord.y = (float)z / (height - 1);
            
            // Calculate proper normals for lighting
            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            if (x > 0 && x < width - 1 && z > 0 && z < height - 1) {
                float hL = heightData[(z) * width + (x - 1)];
                float hR = heightData[(z) * width + (x + 1)];
                float hD = heightData[(z - 1) * width + x];
                float hU = heightData[(z + 1) * width + x];
                
                normal.x = (hL - hR) * 0.5f;
                normal.z = (hD - hU) * 0.5f;
                normal.y = 2.0f;
                normal = glm::normalize(normal);
            }
            vertex.normal = normal;
            
                        // Realistic terrain colors with strong contrast
            float normalizedHeight = (vertex.position.y + 50.0f) / 150.0f;
            normalizedHeight = std::clamp(normalizedHeight, 0.0f, 1.0f);
            
            if (normalizedHeight < 0.1f) {
                vertex.color = glm::vec3(0.0f, 0.1f, 0.6f); // Deep water - dark blue
            } else if (normalizedHeight < 0.2f) {
                vertex.color = glm::vec3(0.1f, 0.3f, 0.7f); // Shallow water - medium blue
            } else if (normalizedHeight < 0.25f) {
                vertex.color = glm::vec3(0.8f, 0.7f, 0.4f); // Beach/sand - tan
            } else if (normalizedHeight < 0.4f) {
                vertex.color = glm::vec3(0.2f, 0.6f, 0.1f); // Low grassland - bright green
            } else if (normalizedHeight < 0.6f) {
                vertex.color = glm::vec3(0.1f, 0.4f, 0.05f); // Hills - dark green
            } else if (normalizedHeight < 0.75f) {
                vertex.color = glm::vec3(0.5f, 0.4f, 0.2f); // Low mountains - brown
            } else if (normalizedHeight < 0.9f) {
                vertex.color = glm::vec3(0.4f, 0.3f, 0.2f); // High mountains - dark brown
            } else {
                vertex.color = glm::vec3(0.9f, 0.9f, 0.95f); // Snow peaks - light gray
            }
            
            m_vertices.push_back(vertex);
        }
    }
    
    // Generate indices
    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int topLeft = z * width + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * width + x;
            int bottomRight = bottomLeft + 1;
            
            m_indices.push_back(topLeft);
            m_indices.push_back(bottomLeft);
            m_indices.push_back(topRight);
            
            m_indices.push_back(topRight);
            m_indices.push_back(bottomLeft);
            m_indices.push_back(bottomRight);
        }
    }
    
    SetupMesh();
}

void TerrainMesh::SetupMesh() {
    glGenVertexArraysAPPLE(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArrayAPPLE(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(TerrainVertex), 
                 m_vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int),
                 m_indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);
    
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), 
                         (void*)offsetof(TerrainVertex, normal));
    
    // Color attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), 
                         (void*)offsetof(TerrainVertex, color));
    
    glBindVertexArrayAPPLE(0);
}

void TerrainMesh::Render() {
    if (m_VAO && !m_indices.empty()) {
        glBindVertexArrayAPPLE(m_VAO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArrayAPPLE(0);
    }
}

TerrainEngine::TerrainEngine() 
    : m_width(0), m_height(0), m_minHeight(0.0f), m_maxHeight(0.0f), m_terrainScale(1.0f) {
    m_terrainMesh = std::make_unique<TerrainMesh>();
}

TerrainEngine::~TerrainEngine() = default;

bool TerrainEngine::LoadTerrain(const std::string& filepath) {
    std::cout << "Loading terrain from: " << filepath << std::endl;
    std::cout << "File loading not yet implemented, generating random terrain instead" << std::endl;
    GenerateRandomTerrain(256, 256);
    return true;
}

void TerrainEngine::GenerateRandomTerrain(int width, int height) {
    std::cout << "Generating high-resolution terrain (" << width << "x" << height << ")" << std::endl;
    
    m_width = width;
    m_height = height;
    m_heightData.resize(width * height);
    
    // Enhanced randomization - ensure completely different terrain each launch
    std::random_device rd;
    auto seed = rd() ^ static_cast<unsigned>(std::time(nullptr)) ^ static_cast<unsigned>(getpid());
    std::mt19937 gen(seed);
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);  // Normalized for better randomization
    std::uniform_real_distribution<float> feature_dis(0.6f, 1.4f);
    
    std::cout << "🌱 Generating unique terrain with seed: " << seed << std::endl;
    
    // Enhanced terrain characteristics for VERY dramatic slopes with extreme elevation
    float base_amplitude = 80.0f + dis(gen) * 60.0f;  // MUCH more dramatic terrain (80-140 for steep mountains)
    float base_frequency = 0.0008f + (rd() % 5000) * 0.0000003f;  // Very low frequency for massive mountain ranges
    float terrain_complexity = 1.2f + feature_dis(gen) * 0.5f;  // Very high complexity for extreme terrain
    
    std::cout << "�️  Enhanced terrain: amplitude=" << base_amplitude 
              << ", frequency=" << base_frequency 
              << ", complexity=" << terrain_complexity << " (dramatic slopes)" << std::endl;
    
    // Generate much more detailed heightmap with multiple octaves
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float height_val = 0.0f;
            
            // Primary terrain features with randomized parameters
            float amplitude = base_amplitude;
            float frequency = base_frequency;
            
            // Generate varied terrain with randomized characteristics
            for (int octave = 0; octave < 5; ++octave) {
                float sample_x = x * frequency;
                float sample_z = z * frequency;
                
                // Enhanced terrain generation with EXTREME variation and steep gradients
                height_val += sin(sample_x * terrain_complexity) * cos(sample_z) * amplitude;
                height_val += cos(sample_x * 1.3f) * sin(sample_z * 0.9f * terrain_complexity) * amplitude * 0.9f;
                height_val += sin(sample_x * 2.1f + terrain_complexity) * amplitude * 0.7f;
                height_val += cos(sample_z * 1.7f + terrain_complexity) * amplitude * 0.6f;  // Additional steepness
                
                // Add sharp ridges and valleys for extreme terrain
                height_val += sin(sample_x * 4.0f) * cos(sample_z * 4.0f) * amplitude * 0.4f;
                
                // Add randomized noise for steep terrain features
                height_val += (dis(gen) * 0.15f) * amplitude * 0.3f;
                
                amplitude *= 0.5f;
                frequency *= 2.0f;
            }
            
            // Add moderate detail noise
            height_val += dis(gen) * 0.15f;
            
            m_heightData[z * width + x] = height_val;
            
            if (x == 0 && z == 0) {
                m_minHeight = m_maxHeight = height_val;
            } else {
                m_minHeight = std::min(m_minHeight, height_val);
                m_maxHeight = std::max(m_maxHeight, height_val);
            }
        }
    }
    
    std::cout << "High-resolution terrain generated - Height range: " << m_minHeight << " to " << m_maxHeight << std::endl;
    
    // Use enhanced vertical scale for steeper appearance
    float steepScale = 2.0f;  // Double the vertical scale for much steeper terrain
    m_terrainMesh->GenerateFromHeightmap(m_heightData, width, height, steepScale);
}

void TerrainEngine::Render() {
    if (m_terrainMesh) {
        // Render the base terrain mesh
        m_terrainMesh->Render();
        
        // Render contour lines on top
        RenderContourLines();
    }
}

void TerrainEngine::RenderContourLines() const {
    if (m_heightData.empty()) return;
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glLineWidth(1.5f);
    
    // Calculate contour interval (every 5 units for detailed mapping)
    float contourInterval = 5.0f;
    int numContours = (int)((m_maxHeight - m_minHeight) / contourInterval) + 1;
    
    for (int c = 0; c < numContours; ++c) {
        float elevation = m_minHeight + c * contourInterval;
        
        // Use different colors for major vs minor contours
        if (c % 5 == 0) {
            // Major contour lines (every 25 units) in brown
            glColor3f(0.6f, 0.3f, 0.1f);
            glLineWidth(2.5f);
        } else {
            // Minor contour lines in lighter brown
            glColor3f(0.4f, 0.25f, 0.1f);
            glLineWidth(1.5f);
        }
        
        // Draw contour lines by sampling the heightmap
        for (int z = 0; z < m_height - 1; ++z) {
            for (int x = 0; x < m_width - 1; ++x) {
                float h1 = m_heightData[z * m_width + x];
                float h2 = m_heightData[z * m_width + (x + 1)];
                float h3 = m_heightData[(z + 1) * m_width + x];
                float h4 = m_heightData[(z + 1) * m_width + (x + 1)];
                
                // Check if contour line crosses any edge of this quad
                bool crosses = false;
                glBegin(GL_LINES);
                
                // Check horizontal edge (top)
                if ((h1 <= elevation && h2 >= elevation) || (h1 >= elevation && h2 <= elevation)) {
                    float t = (elevation - h1) / (h2 - h1);
                    float worldX = (x + t - m_width * 0.5f) * m_terrainScale;
                    float worldZ = (z - m_height * 0.5f) * m_terrainScale;
                    glVertex3f(worldX, elevation * m_terrainScale, worldZ);
                    crosses = true;
                }
                
                // Check vertical edge (left)
                if ((h1 <= elevation && h3 >= elevation) || (h1 >= elevation && h3 <= elevation)) {
                    float t = (elevation - h1) / (h3 - h1);
                    float worldX = (x - m_width * 0.5f) * m_terrainScale;
                    float worldZ = (z + t - m_height * 0.5f) * m_terrainScale;
                    if (crosses) {
                        glVertex3f(worldX, elevation * m_terrainScale, worldZ);
                    } else {
                        glVertex3f(worldX, elevation * m_terrainScale, worldZ);
                        crosses = true;
                    }
                }
                
                // Check horizontal edge (bottom)
                if ((h3 <= elevation && h4 >= elevation) || (h3 >= elevation && h4 <= elevation)) {
                    float t = (elevation - h3) / (h4 - h3);
                    float worldX = (x + t - m_width * 0.5f) * m_terrainScale;
                    float worldZ = (z + 1 - m_height * 0.5f) * m_terrainScale;
                    if (crosses) {
                        glVertex3f(worldX, elevation * m_terrainScale, worldZ);
                    } else {
                        glVertex3f(worldX, elevation * m_terrainScale, worldZ);
                        crosses = true;
                    }
                }
                
                // Check vertical edge (right)
                if ((h2 <= elevation && h4 >= elevation) || (h2 >= elevation && h4 <= elevation)) {
                    float t = (elevation - h2) / (h4 - h2);
                    float worldX = (x + 1 - m_width * 0.5f) * m_terrainScale;
                    float worldZ = (z + t - m_height * 0.5f) * m_terrainScale;
                    if (crosses) {
                        glVertex3f(worldX, elevation * m_terrainScale, worldZ);
                    }
                }
                
                glEnd();
            }
        }
    }
    
    // Draw elevation labels for major contours
    glColor3f(0.8f, 0.4f, 0.2f);
    for (int c = 0; c < numContours; c += 5) {
        float elevation = m_minHeight + c * contourInterval;
        // Add elevation text rendering here if needed
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glLineWidth(1.0f);
}

float TerrainEngine::GetElevationAt(float x, float z) const {
    int gridX = (int)(x + m_width * 0.5f);
    int gridZ = (int)(z + m_height * 0.5f);
    
    if (gridX >= 0 && gridX < m_width && gridZ >= 0 && gridZ < m_height) {
        return m_heightData[gridZ * m_width + gridX];
    }
    return 0.0f;
}

bool TerrainEngine::HasLineOfSight(const glm::vec3& from, const glm::vec3& to) const {
    return true; // Simplified for now
}

glm::vec3 TerrainEngine::GetTerrainSize() const {
    return glm::vec3(m_width, m_maxHeight - m_minHeight, m_height);
}

}
