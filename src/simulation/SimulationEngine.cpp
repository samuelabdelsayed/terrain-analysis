#include "simulation/SimulationEngine.h"
#include <iostream>
#include <algorithm>

namespace TS {

SimulationEngine::SimulationEngine() 
    : m_state(SimulationState::STOPPED), m_simulationTime(0.0f), m_nextUnitId(1) {
}

SimulationEngine::~SimulationEngine() {
    // Safe cleanup - clear vector will properly destroy unique_ptrs
    Reset();
}

void SimulationEngine::Initialize() {
    std::cout << "🎮 Initializing Dynamic Simulation Engine..." << std::endl;
    Reset();
    
    // Create initial scenario with strategic positioning
    CreateScenario("Border Patrol");
    
    std::cout << "🎯 Dynamic simulation initialized with " << m_units.size() << " units" << std::endl;
    std::cout << "📊 Scenario: Active patrol and reconnaissance mission" << std::endl;
}

void SimulationEngine::CreateScenario(const std::string& scenarioName) {
    std::cout << "🎬 Creating scenario: " << scenarioName << std::endl;
    
    if (scenarioName == "Border Patrol") {
        // Allied patrol formation - spawn well above terrain with high elevation
        AddUnit(UnitType::PERSONNEL, glm::vec3(-20, 800, -20), true);
        AddUnit(UnitType::VEHICLE, glm::vec3(-15, 800, -25), true);
        AddUnit(UnitType::SENSOR, glm::vec3(-25, 800, -15), true);
        
        // Opposition reconnaissance - spawn well above terrain with high elevation
        AddUnit(UnitType::PERSONNEL, glm::vec3(20, 800, 20), false);
        AddUnit(UnitType::VEHICLE, glm::vec3(15, 800, 25), false);
        AddUnit(UnitType::EQUIPMENT, glm::vec3(25, 800, 15), false);
    }
}

void SimulationEngine::Update(float deltaTime) {
    if (m_state != SimulationState::RUNNING) {
        return;
    }
    
    m_simulationTime += deltaTime;
    
    // Track unit activity for feedback
    static float activityTimer = 0.0f;
    activityTimer += deltaTime;
    
    int unitsMoving = 0;
    int unitsInContact = 0;
    
    // Update all units and check for engagements
    for (auto& unit : m_units) {
        if (unit) { // Safety check
            unit->Update(deltaTime);
            
            // Check for engagements with other units
            unit->CheckContact(m_units, deltaTime);
            
            // Check if unit is moving
            auto pos = unit->GetPosition();
            auto target = unit->GetTargetPosition();
            if (glm::distance(pos, target) > 1.0f) {
                unitsMoving++;
            }
            
            // Check if unit has an active command
            if (!unit->GetActiveCommand().empty()) {
                unitsInContact++;
            }
        }
    }
    
    // Report significant activity every 8 seconds
    if (activityTimer >= 8.0f) {
        if (unitsMoving > 0 || unitsInContact > 0) {
            std::cout << "⚡ FIELD ACTIVITY: " << unitsMoving << " units maneuvering, " 
                      << unitsInContact << " executing instructions" << std::endl;
            if (unitsMoving >= 3) {
                std::cout << "🚁 Heavy movement detected across multiple sectors" << std::endl;
                system("afplay /System/Library/Sounds/Blow.aiff > /dev/null 2>&1 &");
            }
        }
        activityTimer = 0.0f;
    }
    
    // Remove inactive units safely
    m_units.erase(
        std::remove_if(m_units.begin(), m_units.end(),
            [](const std::unique_ptr<Unit>& unit) {
                return !unit || !unit->IsActive();
            }),
        m_units.end()
    );
}

void SimulationEngine::Reset() {
    // Safe cleanup - let unique_ptr destructors handle memory
    m_units.clear();
    m_simulationTime = 0.0f;
    m_nextUnitId = 1;
    m_state = SimulationState::STOPPED;
}

int SimulationEngine::AddUnit(UnitType type, const glm::vec3& position, bool isAllied) {
    int unitId = m_nextUnitId++;
    
    // Create unit safely
    auto unit = std::make_unique<Unit>(unitId, type, position, isAllied);
    m_units.push_back(std::move(unit));
    
    std::cout << "Added " << (isAllied ? "allied" : "opposition") << " unit " << unitId 
              << " at (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    
    return unitId;
}

Unit* SimulationEngine::GetUnit(int unitId) {
    for (auto& unit : m_units) {
        if (unit && unit->GetId() == unitId) {
            return unit.get();
        }
    }
    return nullptr;
}

std::vector<Unit*> SimulationEngine::GetAllUnits() const {
    std::vector<Unit*> units;
    units.reserve(m_units.size());
    
    for (const auto& unit : m_units) {
        if (unit) { // Safety check
            units.push_back(unit.get());
        }
    }
    return units;
}

}
