#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <string>

namespace TS {

class Camera;
class TerrainEngine;
class SimulationEngine;
class DatabaseManager;
class AISystem;

class Application {
    GLFWwindow* m_window;
    bool m_isRunning;
    float m_lastFrameTime;
    
    // Advanced components
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<TerrainEngine> m_terrainEngine;
    std::unique_ptr<SimulationEngine> m_simulationEngine;
    std::unique_ptr<DatabaseManager> m_database;
    std::unique_ptr<AISystem> m_aiSystem;
    
    // Input state
    bool m_keys[1024];
    double m_lastMouseX, m_lastMouseY;
    bool m_firstMouse;
    
    // Visual feedback for operator commands
    std::string m_lastCommand;
    float m_commandFeedbackTimer;
    int m_commandExecutionCount;
    
public:
    Application();
    ~Application();
    bool Initialize();
    void Run();
    void Shutdown();
    void Update(float deltaTime);
    void Render();
    void HandleInput();
    void ProcessKeyboard(float deltaTime);
    void ProcessMouse(double xpos, double ypos);
    void CommandBlueForces(const std::string& command);
    void RenderContoured3DGrid();
    
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
};

}
