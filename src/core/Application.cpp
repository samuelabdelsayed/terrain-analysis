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
      m_lastMouseX(640), m_lastMouseY(360), m_firstMouse(true),
      m_lastCommand(""), m_commandFeedbackTimer(0.0f), m_commandExecutionCount(0) {
    
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
    std::cout << "\n� BLUE FORCE OPERATOR COMMANDS:" << std::endl;
    std::cout << "  1: Advance and secure area" << std::endl;
    std::cout << "  2: Take defensive positions" << std::endl;
    std::cout << "  3: Begin patrol operations" << std::endl;
    std::cout << "  4: Withdraw to rally point" << std::endl;
    std::cout << "  5: Reconnaissance mode" << std::endl;
    std::cout << "\n🚀 Enhanced Features:" << std::endl;
    std::cout << "  📊  3D contoured grid terrain (no white mesh)" << std::endl;
    std::cout << "  🧠  AI learning and strategy adaptation" << std::endl;
    std::cout << "  🔵  Interactive blue force command and control" << std::endl;
    std::cout << "  🎯  Real-time tactical decision making" << std::endl;
    std::cout << "\n💡 OPERATOR GUIDANCE:" << std::endl;
    std::cout << "  • YOU command the BLUE forces (rectangles)" << std::endl;
    std::cout << "  • RED forces are AI-controlled (diamonds)" << std::endl;
    std::cout << "  • Use keys 1-5 to give tactical orders" << std::endl;
    std::cout << "  • Watch blue units respond to your commands" << std::endl;
    std::cout << "  • AI will counter your moves with red forces" << std::endl;
    
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
        
        // Update command feedback timer
        if (m_commandFeedbackTimer > 0.0f) {
            m_commandFeedbackTimer -= deltaTime;
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
    
    // Render 3D contoured grid terrain (NO white mesh, NO orange contours)
    if (m_terrainEngine && m_terrainEngine->IsLoaded()) {
        try {
            // COMPLETELY SKIP the white terrain mesh rendering
            // m_terrainEngine->Render(); // DISABLED!
            
            // Render ONLY the 3D contoured green grid
            glDisable(GL_LIGHTING);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            // Simple, clean 3D grid that follows terrain elevation (no chaotic contours)
            const int gridSize = 100;
            const int gridSpacing = 10;
            
            // Clean grid lines that follow terrain elevation
            glLineWidth(1.0f);
            glColor4f(0.0f, 0.7f, 0.0f, 0.8f);
            
            // Horizontal grid lines
            for (int x = -gridSize; x <= gridSize; x += gridSpacing) {
                glBegin(GL_LINE_STRIP);
                for (int z = -gridSize; z <= gridSize; z += gridSpacing) {
                    float elevation = m_terrainEngine->GetElevationAt(x, z);
                    glVertex3f(x, elevation * 0.5f + 2.0f, z);
                }
                glEnd();
            }
            
            // Vertical grid lines
            for (int z = -gridSize; z <= gridSize; z += gridSpacing) {
                glBegin(GL_LINE_STRIP);
                for (int x = -gridSize; x <= gridSize; x += gridSpacing) {
                    float elevation = m_terrainEngine->GetElevationAt(x, z);
                    glVertex3f(x, elevation * 0.5f + 2.0f, z);
                }
                glEnd();
            }
            
            // Simple coordinate reference lines
            glColor4f(0.0f, 0.5f, 0.0f, 0.6f);
            glLineWidth(1.5f);
            glBegin(GL_LINES);
            // Main axes
            glVertex3f(-gridSize, 1, 0); glVertex3f(gridSize, 1, 0);
            glVertex3f(0, 1, -gridSize); glVertex3f(0, 1, gridSize);
            glEnd();
            
            // NO MORE CHAOTIC CONTOURS - Keep it simple and clean
            
            glEnable(GL_LIGHTING);
            
        } catch (const std::exception& e) {
            std::cerr << "Error rendering 3D grid: " << e.what() << std::endl;
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
        
        // NO MORE BROWN/ORANGE CONTOURS - All visualization is now green 3D grid only
        
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
                    glm::vec3 pos = unit->GetPosition();
                    
                    // Enhanced visual feedback for units executing commands
                    if (unit->HasActiveCommand()) {
                        // Bright pulsing outline for units with active commands
                        float pulse = 0.7f + 0.3f * sin(glfwGetTime() * 8.0f);
                        if (unit->IsAllied()) {
                            glColor4f(0.0f, 1.0f, 1.0f, pulse); // Cyan for blue force commands
                        } else {
                            glColor4f(1.0f, 0.5f, 0.0f, pulse); // Orange for red force
                        }
                        glLineWidth(10.0f);
                        EntitySymbols::RenderUnitSymbol(*unit);
                        
                        // Command-specific visual indicators above unit
                        std::string activeCmd = unit->GetActiveCommand();
                        glLineWidth(3.0f);
                        glPushMatrix();
                        glTranslatef(pos.x, pos.y + 8.0f, pos.z);
                        
                        if (activeCmd == "ADVANCING") {
                            glColor4f(0.0f, 1.0f, 0.0f, pulse); // Green arrow
                            glBegin(GL_TRIANGLES);
                            glVertex3f(0, 0, 2); glVertex3f(-1, 0, 0); glVertex3f(1, 0, 0);
                            glEnd();
                        } else if (activeCmd == "DEFENDING") {
                            glColor4f(1.0f, 1.0f, 0.0f, pulse); // Yellow shield
                            glBegin(GL_LINE_LOOP);
                            glVertex3f(0, 0, 1); glVertex3f(-1, 0, 0); glVertex3f(0, 0, -1); glVertex3f(1, 0, 0);
                            glEnd();
                        } else if (activeCmd == "PATROLLING") {
                            glColor4f(0.0f, 0.5f, 1.0f, pulse); // Blue circle
                            glBegin(GL_LINE_LOOP);
                            for (int i = 0; i < 12; i++) {
                                float angle = i * M_PI / 6.0f;
                                glVertex3f(cos(angle), 0, sin(angle));
                            }
                            glEnd();
                        } else if (activeCmd == "WITHDRAWING") {
                            glColor4f(1.0f, 0.5f, 0.0f, pulse); // Orange retreat arrow
                            glBegin(GL_TRIANGLES);
                            glVertex3f(0, 0, -2); glVertex3f(-1, 0, 0); glVertex3f(1, 0, 0);
                            glEnd();
                        } else if (activeCmd == "RECON") {
                            glColor4f(1.0f, 0.0f, 1.0f, pulse); // Magenta eye
                            glBegin(GL_LINE_LOOP);
                            glVertex3f(-1, 0, 0); glVertex3f(0, 0, 1); glVertex3f(1, 0, 0); glVertex3f(0, 0, -1);
                            glEnd();
                        }
                        glPopMatrix();
                    }
                    
                    // Standard glow effect around symbols
                    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
                    glLineWidth(6.0f);
                    EntitySymbols::RenderUnitSymbol(*unit);
                    
                    // Render main symbol
                    glLineWidth(4.0f);
                    EntitySymbols::RenderUnitSymbol(*unit);
                }
            }
            
            glDisable(GL_BLEND);
            glEnable(GL_LIGHTING);
            
            // Render visual command feedback
            if (m_commandFeedbackTimer > 0.0f && !m_lastCommand.empty()) {
                glDisable(GL_LIGHTING);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                // Calculate fade effect
                float alpha = m_commandFeedbackTimer / 3.0f;
                
                // Draw command feedback in the center of screen (HUD style)
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
                glOrtho(0, 1280, 720, 0, -1, 1);  // Screen coordinates
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();
                
                // Bright cyan background for command feedback
                glColor4f(0.0f, 1.0f, 1.0f, alpha * 0.8f);
                glBegin(GL_QUADS);
                glVertex2f(400, 50);
                glVertex2f(880, 50);
                glVertex2f(880, 120);
                glVertex2f(400, 120);
                glEnd();
                
                // Command text outline
                glColor4f(0.0f, 0.0f, 0.0f, alpha);
                glLineWidth(3.0f);
                glBegin(GL_LINE_LOOP);
                glVertex2f(400, 50);
                glVertex2f(880, 50);
                glVertex2f(880, 120);
                glVertex2f(400, 120);
                glEnd();
                
                // Visual indicators for command type
                if (m_lastCommand == "ADVANCE") {
                    glColor4f(0.0f, 1.0f, 0.0f, alpha);  // Green
                    // Draw arrow pointing forward
                    glBegin(GL_TRIANGLES);
                    glVertex2f(420, 85);
                    glVertex2f(450, 70);
                    glVertex2f(450, 100);
                    glEnd();
                } else if (m_lastCommand == "DEFEND") {
                    glColor4f(1.0f, 1.0f, 0.0f, alpha);  // Yellow
                    // Draw shield shape
                    glBegin(GL_POLYGON);
                    glVertex2f(435, 70);
                    glVertex2f(420, 85);
                    glVertex2f(435, 100);
                    glVertex2f(450, 85);
                    glEnd();
                } else if (m_lastCommand == "PATROL") {
                    glColor4f(0.0f, 0.5f, 1.0f, alpha);  // Blue
                    // Draw circular patrol indicator
                    glBegin(GL_LINE_STRIP);
                    for (int i = 0; i <= 360; i += 30) {
                        float rad = i * M_PI / 180.0f;
                        glVertex2f(435 + cos(rad) * 15, 85 + sin(rad) * 15);
                    }
                    glEnd();
                } else if (m_lastCommand == "WITHDRAW") {
                    glColor4f(1.0f, 0.5f, 0.0f, alpha);  // Orange
                    // Draw arrow pointing back
                    glBegin(GL_TRIANGLES);
                    glVertex2f(450, 85);
                    glVertex2f(420, 70);
                    glVertex2f(420, 100);
                    glEnd();
                } else if (m_lastCommand == "RECON") {
                    glColor4f(1.0f, 0.0f, 1.0f, alpha);  // Magenta
                    // Draw eye shape
                    glBegin(GL_LINE_LOOP);
                    glVertex2f(420, 85);
                    glVertex2f(435, 75);
                    glVertex2f(450, 85);
                    glVertex2f(435, 95);
                    glEnd();
                }
                
                // Restore matrices
                glPopMatrix();
                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
                
                glDisable(GL_BLEND);
                glEnable(GL_LIGHTING);
            }
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
            std::cout << "🔄 Simulation reset with new scenario" << std::endl;
        } else if (key == GLFW_KEY_1) {
            std::cout << "🔵 BLUE FORCE COMMAND: Advance and secure area" << std::endl;
            app->CommandBlueForces("ADVANCE");
        } else if (key == GLFW_KEY_2) {
            std::cout << "🔵 BLUE FORCE COMMAND: Take defensive positions" << std::endl;
            app->CommandBlueForces("DEFEND");
        } else if (key == GLFW_KEY_3) {
            std::cout << "🔵 BLUE FORCE COMMAND: Begin patrol operations" << std::endl;
            app->CommandBlueForces("PATROL");
        } else if (key == GLFW_KEY_4) {
            std::cout << "🔵 BLUE FORCE COMMAND: Withdraw to rally point" << std::endl;
            app->CommandBlueForces("WITHDRAW");
        } else if (key == GLFW_KEY_5) {
            std::cout << "🔵 BLUE FORCE COMMAND: Reconnaissance mode" << std::endl;
            app->CommandBlueForces("RECON");
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

void Application::CommandBlueForces(const std::string& command) {
    if (!m_simulationEngine) {
        std::cout << "⚠️  No simulation engine available for blue force commands" << std::endl;
        return;
    }
    
    std::cout << "🔵 EXECUTING BLUE FORCE COMMAND: " << command << std::endl;
    
    // Set visual feedback variables
    m_lastCommand = command;
    m_commandFeedbackTimer = 3.0f; // Show feedback for 3 seconds
    m_commandExecutionCount++;
    
    // Get all units from simulation
    auto units = m_simulationEngine->GetAllUnits();
    int blueUnitsAffected = 0;
    
    for (auto& unit : units) {
        // Only command blue/allied units
        if (unit->IsAllied()) {
            
            if (command == "ADVANCE") {
                // Move toward center/objective
                unit->SetTargetPosition(glm::vec3(10.0f, 0.0f, 10.0f));
                unit->SetMovementSpeed(2.5f); // Faster movement
                unit->SetActiveCommand("ADVANCING", 4.0f); // Visual feedback for 4 seconds
                std::cout << "  ➡️  " << unit->GetTypeString() << " advancing to objective" << std::endl;
                
            } else if (command == "DEFEND") {
                // Hold current position with defensive stance
                auto currentPos = unit->GetPosition();
                unit->SetTargetPosition(currentPos); // Stay in place
                unit->SetMovementSpeed(0.8f); // Slower, cautious movement
                unit->SetActiveCommand("DEFENDING", 4.0f); // Visual feedback
                std::cout << "  🛡️  " << unit->GetTypeString() << " taking defensive position" << std::endl;
                
            } else if (command == "PATROL") {
                // Begin patrol pattern around current area
                auto currentPos = unit->GetPosition();
                float patrolRadius = 25.0f;
                glm::vec3 patrolTarget = currentPos + glm::vec3(
                    sin(glfwGetTime() + unit->GetId()) * patrolRadius,
                    0.0f,
                    cos(glfwGetTime() + unit->GetId()) * patrolRadius
                );
                unit->SetTargetPosition(patrolTarget);
                unit->SetMovementSpeed(1.8f); // Normal patrol speed
                unit->SetActiveCommand("PATROLLING", 4.0f); // Visual feedback
                std::cout << "  🔄  " << unit->GetTypeString() << " beginning patrol operations" << std::endl;
                
            } else if (command == "WITHDRAW") {
                // Move to safe rally point (back corner)
                unit->SetTargetPosition(glm::vec3(-40.0f, 0.0f, -40.0f));
                unit->SetMovementSpeed(3.0f); // Fast withdrawal
                unit->SetActiveCommand("WITHDRAWING", 4.0f); // Visual feedback
                std::cout << "  ⬅️  " << unit->GetTypeString() << " withdrawing to rally point" << std::endl;
                
            } else if (command == "RECON") {
                // Scout ahead toward enemy positions
                unit->SetTargetPosition(glm::vec3(50.0f, 0.0f, 30.0f));
                unit->SetMovementSpeed(1.2f); // Slow, stealthy movement
                unit->SetActiveCommand("RECON", 4.0f); // Visual feedback
                std::cout << "  🔍  " << unit->GetTypeString() << " conducting reconnaissance" << std::endl;
            }
            
            blueUnitsAffected++;
        }
    }
    
    std::cout << "✅ Command executed - " << blueUnitsAffected << " blue force units received orders" << std::endl;
    
    // Notify AI system that player has issued commands
    if (m_aiSystem) {
        m_aiSystem->ReactToPlayerCommand(command);
    }
}

}
