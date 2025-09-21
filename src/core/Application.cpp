#include "core/Application.h"
#include "graphics/Camera.h"
#include "graphics/EntitySymbols.h"
#include "terrain/TerrainEngine.h"
#include "simulation/SimulationEngine.h"
#include "data/DatabaseManager.h"
#include "ai/AISystem.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <cstring>

#include <OpenGL/gl.h>

namespace TS {

// Manual perspective function to replace gluPerspective
void SetPerspective(float fovy, float aspect, float nearPlane, float farPlane) {
    float ymax = nearPlane * tan(fovy * M_PI / 360.0);
    float ymin = -ymax;
    float xmin = ymin * aspect;
    float xmax = ymax * aspect;
    
    glFrustum(xmin, xmax, ymin, ymax, nearPlane, farPlane);
}

// Manual lookAt function to replace gluLookAt
void SetLookAt(float eyeX, float eyeY, float eyeZ,
               float centerX, float centerY, float centerZ,
               float upX, float upY, float upZ) {
    
    // Calculate forward vector
    float fx = centerX - eyeX;
    float fy = centerY - eyeY;
    float fz = centerZ - eyeZ;
    
    // Normalize forward
    float flen = sqrt(fx*fx + fy*fy + fz*fz);
    fx /= flen; fy /= flen; fz /= flen;
    
    // Calculate right vector (forward x up)
    float rx = fy*upZ - fz*upY;
    float ry = fz*upX - fx*upZ;
    float rz = fx*upY - fy*upX;
    
    // Normalize right
    float rlen = sqrt(rx*rx + ry*ry + rz*rz);
    rx /= rlen; ry /= rlen; rz /= rlen;
    
    // Calculate up vector (right x forward)
    float ux = ry*fz - rz*fy;
    float uy = rz*fx - rx*fz;
    float uz = rx*fy - ry*fx;
    
    // Create transformation matrix
    float m[16] = {
        rx, ux, -fx, 0,
        ry, uy, -fy, 0,
        rz, uz, -fz, 0,
        0,  0,  0,   1
    };
    
    glMultMatrixf(m);
    glTranslatef(-eyeX, -eyeY, -eyeZ);
}

Application::Application() 
    : m_window(nullptr), m_isRunning(false), m_lastFrameTime(0.0f),
      m_lastMouseX(640), m_lastMouseY(360), m_firstMouse(true) {
    
    std::memset(m_keys, 0, sizeof(m_keys));
}

Application::~Application() {
    Shutdown();
}

bool Application::Initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    std::cout << "GLFW initialized successfully" << std::endl;
    
    // Use compatibility profile for legacy OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    
    m_window = glfwCreateWindow(1280, 720, "Terrain Simulator - Enhanced", nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(m_window);
    std::cout << "Window created successfully (1280x720)" << std::endl;
    
    // Set callbacks
    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, KeyCallback);
    glfwSetCursorPosCallback(m_window, MouseCallback);
    
    // Mouse starts free
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    // Enhanced OpenGL setup with darker background
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    
    // Set up lighting for better terrain visibility
    float lightPos[] = {100.0f, 300.0f, 100.0f, 1.0f};
    float lightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f}; // Darker ambient
    float lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Bright diffuse
    float lightSpecular[] = {0.5f, 0.5f, 0.5f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    
    // Professional topographical map background\n    glClearColor(0.25f, 0.35f, 0.45f, 1.0f); // Much lighter blue-gray for excellent contrast
    
    // Initialize components
    try {
        std::cout << "Initializing Camera..." << std::endl;
        m_camera = std::make_unique<Camera>(glm::vec3(0, 80, 120)); // Better starting position
        
        std::cout << "Initializing Terrain..." << std::endl;
        m_terrainEngine = std::make_unique<TerrainEngine>();
        m_terrainEngine->GenerateRandomTerrain(128, 128);
        
        std::cout << "Initializing Simulation Engine..." << std::endl;
        m_simulationEngine = std::make_unique<SimulationEngine>();
        m_simulationEngine->Initialize();
        
        std::cout << "Initializing Database..." << std::endl;
        m_database = std::make_unique<DatabaseManager>();
        m_database->Initialize("scenarios.db");
        
        std::cout << "Initializing AI..." << std::endl;
        m_aiSystem = std::make_unique<AISystem>();
        m_aiSystem->Initialize();
        
    } catch (const std::exception& e) {
        std::cerr << "Error initializing components: " << e.what() << std::endl;
        return false;
    }
    
    std::cout << "\n=== Enhanced Terrain Simulator ===" << std::endl;
    std::cout << "✅ All systems initialized successfully!" << std::endl;
    std::cout << "\n🎮 Controls:" << std::endl;
    std::cout << "  TAB: Toggle mouse capture" << std::endl;
    std::cout << "  Arrow Keys: Move camera (←↑→↓)" << std::endl;
    std::cout << "  Page Up/Down: Move up/down" << std::endl;
    std::cout << "  Mouse: Look around (when captured)" << std::endl;
    std::cout << "  Space: Start/Pause simulation" << std::endl;
    std::cout << "  R: Reset simulation" << std::endl;
    std::cout << "  ESC: Exit safely" << std::endl;
    std::cout << "\n🚀 Enhanced Features:" << std::endl;
    std::cout << "  📊  Professional entity symbols" << std::endl;
    std::cout << "  🗻  High-resolution terrain with realistic colors" << std::endl;
    std::cout << "  �  Military-grade contour mapping" << std::endl;
    std::cout << "  �  Interactive 3D navigation" << std::endl;
    std::cout << "\n💡 INTERACTION TIPS:" << std::endl;
    std::cout << "  • Press TAB to capture mouse and look around" << std::endl;
    std::cout << "  • Use arrow keys to move around the terrain" << std::endl;
    std::cout << "  • Page Up/Down to change elevation" << std::endl;
    std::cout << "  • Space to start/pause unit simulation" << std::endl;
    std::cout << "  • R to reset and generate new terrain" << std::endl;
    
    m_isRunning = true;
    return true;
}

void Application::Run() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    // Start simulation
    if (m_simulationEngine) {
        m_simulationEngine->Start();
    }
    
    while (m_isRunning && !glfwWindowShouldClose(m_window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        glfwPollEvents();
        ProcessKeyboard(deltaTime);
        Update(deltaTime);
        Render();
        
        glfwSwapBuffers(m_window);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void Application::Update(float deltaTime) {
    try {
        if (m_simulationEngine) {
            m_simulationEngine->Update(deltaTime);
        }
        
        if (m_aiSystem) {
            m_aiSystem->Update(deltaTime);
        }
        
        // Print status every 10 seconds
        static float statusTimer = 0.0f;
        statusTimer += deltaTime;
        if (statusTimer >= 10.0f) {
            if (m_simulationEngine) {
                auto units = m_simulationEngine->GetAllUnits();
                int activeUnits = 0;
                for (const auto& unit : units) {
                    if (unit && unit->IsActive()) activeUnits++;
                }
                
                std::cout << "🎯 Active Units: " << activeUnits 
                          << " | Sim Time: " << m_simulationEngine->GetSimulationTime() << "s" << std::endl;
            }
            statusTimer = 0.0f;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in Update: " << e.what() << std::endl;
    }
}

void Application::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    SetPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 2000.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    if (m_camera) {
        glm::vec3 pos = m_camera->GetPosition();
        glm::vec3 front = m_camera->GetFront();
        glm::vec3 target = pos + front;
        
        SetLookAt(pos.x, pos.y, pos.z,
                  target.x, target.y, target.z,
                  0.0f, 1.0f, 0.0f);
    }
    
    // Render terrain with enhanced graphics
    if (m_terrainEngine && m_terrainEngine->IsLoaded()) {
        try {
            // Render base terrain mesh
            m_terrainEngine->Render();
            
            // Now render military-grade overlays on top
            glDisable(GL_LIGHTING);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            // Render coordinate grid system
            glColor4f(0.2f, 0.5f, 0.1f, 0.7f);
            glLineWidth(0.8f);
            glBegin(GL_LINES);
            for (int i = -32; i <= 32; i += 4) {
                float pos = i * 4.0f;
                glVertex3f(-128, 5, pos); glVertex3f(128, 5, pos);
                glVertex3f(pos, 5, -128); glVertex3f(pos, 5, 128);
            }
            glEnd();
            
            // Major grid lines
            glColor4f(0.1f, 0.8f, 0.2f, 0.8f);
            glLineWidth(1.5f);
            glBegin(GL_LINES);
            for (int i = -32; i <= 32; i += 16) {
                float pos = i * 4.0f;
                glVertex3f(-128, 6, pos); glVertex3f(128, 6, pos);
                glVertex3f(pos, 6, -128); glVertex3f(pos, 6, 128);
            }
            glEnd();
            
            // Professional contour lines - much less dense
            for (int elevation = 10; elevation <= 100; elevation += 10) {
                if (elevation % 50 == 0) {
                    // Index contours (every 50m) - thick brown
                    glColor4f(0.6f, 0.3f, 0.1f, 0.8f);
                    glLineWidth(2.5f);
                } else if (elevation % 25 == 0) {
                    // Intermediate contours (every 25m) - medium brown
                    glColor4f(0.5f, 0.25f, 0.1f, 0.7f);
                    glLineWidth(2.0f);
                } else {
                    // Minor contours (every 10m) - light brown
                    glColor4f(0.4f, 0.2f, 0.1f, 0.6f);
                    glLineWidth(1.5f);
                }
                
                // Simple, readable contour shapes
                glBegin(GL_LINE_STRIP);
                for (int angle = 0; angle <= 360; angle += 6) {
                    float rad = angle * M_PI / 180.0f;
                    float radius = 30.0f + elevation * 0.8f + sin(rad * 2) * 8.0f;
                    float x = cos(rad) * radius + sin(elevation * 0.02f) * 15.0f;
                    float z = sin(rad) * radius + cos(elevation * 0.03f) * 12.0f;
                    float y = elevation * 0.7f + 10.0f;
                    glVertex3f(x, y, z);
                }
                glEnd();
            }
            
            glDisable(GL_BLEND);
            glEnable(GL_LIGHTING);
            
        } catch (const std::exception& e) {
            std::cerr << "Error rendering terrain: " << e.what() << std::endl;
        }
    } else {
        // Draw military-grade topographical map with detailed contours
        glDisable(GL_LIGHTING);
        
        // UTM-style coordinate grid in olive green
        glColor3f(0.3f, 0.4f, 0.2f);
        glLineWidth(0.5f);
        glBegin(GL_LINES);
        for (int i = -200; i <= 200; i += 5) {
            glVertex3f(-200, 0, i); glVertex3f(200, 0, i);
            glVertex3f(i, 0, -200); glVertex3f(i, 0, 200);
        }
        glEnd();
        
        // Major grid lines (100m intervals) in darker green
        glColor3f(0.2f, 0.5f, 0.1f);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        for (int i = -200; i <= 200; i += 25) {
            glVertex3f(-200, 0, i); glVertex3f(200, 0, i);
            glVertex3f(i, 0, -200); glVertex3f(i, 0, 200);
        }
        glEnd();
        
        // Primary coordinate lines (1000m intervals) in bright green
        glColor3f(0.1f, 0.8f, 0.2f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        for (int i = -200; i <= 200; i += 100) {
            glVertex3f(-200, 0, i); glVertex3f(200, 0, i);
            glVertex3f(i, 0, -200); glVertex3f(i, 0, 200);
        }
        glEnd();
        
        // Detailed elevation contour lines (military standard)
        glLineWidth(1.5f);
        for (int elevation = 5; elevation <= 100; elevation += 5) {
            if (elevation % 25 == 0) {
                // Index contours (every 25m) in thick brown
                glColor3f(0.6f, 0.3f, 0.1f);
                glLineWidth(2.5f);
            } else if (elevation % 10 == 0) {
                // Intermediate contours (every 10m) in medium brown
                glColor3f(0.5f, 0.35f, 0.15f);
                glLineWidth(2.0f);
            } else {
                // Supplementary contours (every 5m) in light brown
                glColor3f(0.4f, 0.3f, 0.2f);
                glLineWidth(1.5f);
            }
            
            // Draw realistic terrain contours
            glBegin(GL_LINE_STRIP);
            for (int angle = 0; angle <= 360; angle += 5) {
                float rad = angle * M_PI / 180.0f;
                float radius = 40.0f + elevation * 1.2f + sin(rad * 3) * 8.0f + cos(rad * 5) * 4.0f;
                float x = cos(rad) * radius + sin(elevation * 0.1f) * 15.0f;
                float z = sin(rad) * radius + cos(elevation * 0.15f) * 12.0f;
                glVertex3f(x, elevation * 0.8f, z);
            }
            glEnd();
            
            // Add secondary contour patterns for complex terrain
            if (elevation % 15 == 0) {
                glBegin(GL_LINE_STRIP);
                for (int angle = 0; angle <= 360; angle += 8) {
                    float rad = angle * M_PI / 180.0f;
                    float radius = 60.0f + elevation * 0.8f + sin(rad * 2) * 10.0f;
                    float x = cos(rad) * radius - 30.0f + cos(elevation * 0.2f) * 10.0f;
                    float z = sin(rad) * radius + 25.0f + sin(elevation * 0.18f) * 8.0f;
                    glVertex3f(x, elevation * 0.8f, z);
                }
                glEnd();
            }
        }
        
        // Draw depression contours (hachured lines for valleys)
        glColor3f(0.3f, 0.2f, 0.1f);
        glLineWidth(1.0f);
        for (int depression = -5; depression >= -25; depression -= 5) {
            glBegin(GL_LINE_STRIP);
            for (int angle = 0; angle <= 360; angle += 10) {
                float rad = angle * M_PI / 180.0f;
                float radius = 25.0f - depression * 0.5f;
                float x = cos(rad) * radius + 50.0f;
                float z = sin(rad) * radius - 40.0f;
                glVertex3f(x, depression * 0.3f, z);
                
                // Add hachure marks pointing downhill
                if (angle % 30 == 0) {
                    glEnd();
                    glBegin(GL_LINES);
                    glVertex3f(x, depression * 0.3f, z);
                    glVertex3f(x + cos(rad) * 3.0f, depression * 0.3f - 1.0f, z + sin(rad) * 3.0f);
                    glEnd();
                    glBegin(GL_LINE_STRIP);
                    glVertex3f(x, depression * 0.3f, z);
                }
            }
            glEnd();
        }
        
        // Add spot elevation markers
        glColor3f(0.8f, 0.4f, 0.1f);
        glPointSize(4.0f);
        glBegin(GL_POINTS);
        glVertex3f(45.0f, 32.0f, -15.0f);  // Hill peak
        glVertex3f(-35.0f, 28.0f, 40.0f);  // Secondary peak
        glVertex3f(15.0f, 18.0f, 65.0f);   // Ridge point
        glVertex3f(-60.0f, 12.0f, -25.0f); // Saddle point
        glEnd();
        
        glEnable(GL_LIGHTING);
    }
    
    // Render units with enhanced military symbols
    if (m_simulationEngine) {
        try {
            auto units = m_simulationEngine->GetAllUnits();
            
            // Disable lighting for symbols and enable better visibility
            glDisable(GL_LIGHTING);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glLineWidth(4.0f);
            
            for (const auto& unit : units) {
                if (unit && unit->IsActive()) {
                    // Add glow effect around symbols
                    glm::vec3 pos = unit->GetPosition();
                    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
                    glLineWidth(8.0f);
                    EntitySymbols::RenderUnitSymbol(*unit);
                    
                    // Render main symbol
                    glLineWidth(4.0f);
                    EntitySymbols::RenderUnitSymbol(*unit);
                }
            }
            
            glDisable(GL_BLEND);
            glEnable(GL_LIGHTING);
            glLineWidth(1.0f);
            
        } catch (const std::exception& e) {
            std::cerr << "Error rendering units: " << e.what() << std::endl;
        }
    }
}

void Application::ProcessKeyboard(float deltaTime) {
    if (m_camera) {
        // Arrow key controls
        if (m_keys[GLFW_KEY_UP])
            m_camera->ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
        if (m_keys[GLFW_KEY_DOWN])
            m_camera->ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
        if (m_keys[GLFW_KEY_LEFT])
            m_camera->ProcessKeyboard(CameraMovement::LEFT, deltaTime);
        if (m_keys[GLFW_KEY_RIGHT])
            m_camera->ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
        if (m_keys[GLFW_KEY_PAGE_UP])
            m_camera->ProcessKeyboard(CameraMovement::UP, deltaTime);
        if (m_keys[GLFW_KEY_PAGE_DOWN])
            m_camera->ProcessKeyboard(CameraMovement::DOWN, deltaTime);
    }
}

void Application::ProcessMouse(double xpos, double ypos) {
    if (m_firstMouse) {
        m_lastMouseX = xpos;
        m_lastMouseY = ypos;
        m_firstMouse = false;
    }
    
    float xoffset = xpos - m_lastMouseX;
    float yoffset = m_lastMouseY - ypos;
    
    m_lastMouseX = xpos;
    m_lastMouseY = ypos;
    
    if (glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED && m_camera) {
        m_camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void Application::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    
    if (action == GLFW_PRESS) {
        app->m_keys[key] = true;
        
        if (key == GLFW_KEY_ESCAPE) {
            std::cout << "ESC pressed - shutting down safely..." << std::endl;
            app->m_isRunning = false;
        } else if (key == GLFW_KEY_TAB) {
            if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                std::cout << "🖱️  Mouse released - press TAB to recapture" << std::endl;
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                std::cout << "🎮 Mouse captured - use arrow keys to move, mouse to look" << std::endl;
            }
        } else if (key == GLFW_KEY_SPACE && app->m_simulationEngine) {
            if (app->m_simulationEngine->GetState() == SimulationState::RUNNING) {
                app->m_simulationEngine->Pause();
                std::cout << "⏸️  Simulation paused" << std::endl;
            } else {
                app->m_simulationEngine->Start();
                std::cout << "▶️  Simulation started" << std::endl;
            }
        } else if (key == GLFW_KEY_R && app->m_simulationEngine) {
            app->m_simulationEngine->Reset();
            app->m_simulationEngine->Initialize();
            std::cout << "🔄 Simulation reset with new units" << std::endl;
        }
    } else if (action == GLFW_RELEASE) {
        app->m_keys[key] = false;
    }
}

void Application::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->ProcessMouse(xpos, ypos);
}

void Application::HandleInput() {
    // Handled by callbacks
}

void Application::Shutdown() {
    std::cout << "Safe shutdown in progress..." << std::endl;
    
    if (m_simulationEngine) {
        m_simulationEngine->Reset();
    }
    
    m_aiSystem.reset();
    m_database.reset();
    m_simulationEngine.reset();
    m_terrainEngine.reset();
    m_camera.reset();
    
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    
    glfwTerminate();
    std::cout << "Shutdown complete" << std::endl;
}

}
