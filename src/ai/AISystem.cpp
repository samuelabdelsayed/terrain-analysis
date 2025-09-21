#include "ai/AISystem.h"
#include <iostream>
#include <cmath>
#include <random>

namespace TS {

AISystem::AISystem() : m_updateTimer(0.0f), m_learningRate(0.1f), m_experience(0) {
}

AISystem::~AISystem() = default;

void AISystem::Initialize() {
    std::cout << "🧠 Advanced AI System initialized with learning capabilities" << std::endl;
    m_strategies.push_back("Aggressive Advance");
    m_strategies.push_back("Defensive Hold");
    m_strategies.push_back("Flanking Maneuver");
    m_strategies.push_back("Strategic Withdrawal");
    m_currentStrategy = 0;
}

void AISystem::Update(float deltaTime) {
    m_updateTimer += deltaTime;
    
    if (m_updateTimer >= 5.0f) {
        // AI learns and adapts every 5 seconds
        m_updateTimer = 0.0f;
        m_experience++;
        
        // Dynamic strategy selection based on experience
        if (m_experience % 10 == 0) {
            LearnAndAdapt();
        }
        
        // Make tactical decisions
        MakeTacticalDecision();
    }
}

void AISystem::SetComplexity(int level) {
    m_learningRate = 0.05f + (level * 0.02f);
    std::cout << "🎯 AI complexity set to level " << level 
              << " (learning rate: " << m_learningRate << ")" << std::endl;
}

void AISystem::LearnAndAdapt() {
    // Simulate AI learning from battlefield conditions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> stratDist(0, m_strategies.size() - 1);
    
    int oldStrategy = m_currentStrategy;
    m_currentStrategy = stratDist(gen);
    
    if (oldStrategy != m_currentStrategy) {
        std::cout << "🧠 AI LEARNING: Switching from '" << m_strategies[oldStrategy] 
                  << "' to '" << m_strategies[m_currentStrategy] 
                  << "' (Experience: " << m_experience << ")" << std::endl;
    }
}

void AISystem::MakeTacticalDecision() {
    std::cout << "🎯 AI Decision: Executing '" << m_strategies[m_currentStrategy] 
              << "' with " << (int)(m_learningRate * 100) << "% adaptation rate" << std::endl;
}

void AISystem::ReactToPlayerCommand(const std::string& command) {
    std::cout << "🔴 AI REACTION: Player used " << command << " - adapting red force strategy" << std::endl;
    
    // AI reacts intelligently to player commands
    if (command == "ADVANCE") {
        std::cout << "  🛡️  Red forces taking defensive positions against blue advance" << std::endl;
        m_currentStrategy = 1; // Defensive Hold
    } else if (command == "DEFEND") {
        std::cout << "  ⚡  Red forces launching aggressive assault on defensive positions" << std::endl;
        m_currentStrategy = 0; // Aggressive Advance
    } else if (command == "PATROL") {
        std::cout << "  🌊  Red forces initiating flanking maneuvers against patrol routes" << std::endl;
        m_currentStrategy = 2; // Flanking Maneuver
    } else if (command == "WITHDRAW") {
        std::cout << "  🏃  Red forces pursuing withdrawing blue forces" << std::endl;
        m_currentStrategy = 0; // Aggressive Advance
    } else if (command == "RECON") {
        std::cout << "  👁️  Red forces concealing positions from reconnaissance" << std::endl;
        m_currentStrategy = 3; // Strategic Withdrawal
    }
    
    // Increase experience when reacting to player
    m_experience += 2;
    std::cout << "  📈  AI experience increased to " << m_experience << std::endl;
}

}
