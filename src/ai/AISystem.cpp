#include "ai/AISystem.h"
#include <iostream>

namespace TS {

AISystem::AISystem() : m_updateTimer(0.0f) {
}

AISystem::~AISystem() = default;

void AISystem::Initialize() {
    std::cout << "AI System initialized" << std::endl;
}

void AISystem::Update(float deltaTime) {
    m_updateTimer += deltaTime;
    
    if (m_updateTimer >= 5.0f) {
        // AI processing every 5 seconds
        m_updateTimer = 0.0f;
    }
}

void AISystem::SetComplexity(int level) {
    std::cout << "AI complexity set to: " << level << std::endl;
}

}
