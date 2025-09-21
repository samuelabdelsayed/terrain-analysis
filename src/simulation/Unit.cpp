#include "simulation/Unit.h"
#include <algorithm>
#include <iostream>

namespace TS {

Unit::Unit(int id, UnitType type, const glm::vec3& position, bool isAllied)
    : m_id(id), m_type(type), m_isAllied(isAllied), m_position(position),
      m_destination(position), m_targetPosition(position), m_state(UnitState::IDLE), 
      m_lastCommand(""), m_commandFeedbackTimer(0.0f), m_commandExecutionCount(0), m_movementSpeed(25.0f) {
    
    switch (m_type) {
        case UnitType::PERSONNEL:
            m_maxHealth = 100.0f;
            break;
        case UnitType::VEHICLE:
            m_maxHealth = 300.0f;
            break;
        case UnitType::EQUIPMENT:
            m_maxHealth = 150.0f;
            break;
        case UnitType::SENSOR:
            m_maxHealth = 80.0f;
            break;
    }
    
    m_health = m_maxHealth;
}

void Unit::Update(float deltaTime) {
    if (!IsActive()) {
        m_state = UnitState::DISABLED;
        return;
    }
    
    // Dynamic AI-driven movement
    static float behaviorTimer = 0.0f;
    behaviorTimer += deltaTime;
    
    if (m_state == UnitState::MOVING || behaviorTimer > 3.0f) {
        // AI decides new destination every 3 seconds
        if (behaviorTimer > 3.0f) {
            behaviorTimer = 0.0f;
            
            // Different behaviors based on unit type and allegiance
            if (m_isAllied) {
                // Allied units patrol in formation
                float patrolRadius = 50.0f;
                float angle = (GetId() * 60.0f + behaviorTimer * 10.0f) * M_PI / 180.0f;
                m_destination = glm::vec3(
                    cos(angle) * patrolRadius - 50.0f,
                    0,
                    sin(angle) * patrolRadius - 50.0f
                );
            } else {
                // Opposition units use aggressive search patterns
                float searchRadius = 80.0f;
                float angle = (GetId() * 90.0f - behaviorTimer * 15.0f) * M_PI / 180.0f;
                m_destination = glm::vec3(
                    cos(angle) * searchRadius + 50.0f,
                    0,
                    sin(angle) * searchRadius + 50.0f
                );
            }
            m_state = UnitState::MOVING;
        }
        
        // Execute movement
        glm::vec3 direction = m_destination - m_position;
        float distance = glm::length(direction);
        
        if (distance > 2.0f) {
            direction = glm::normalize(direction);
            
            // Use configurable movement speed or default by type
            float speed = m_movementSpeed;
            if (speed == 25.0f) { // If default speed, use type-specific speeds
                switch (m_type) {
                    case UnitType::PERSONNEL: speed = 20.0f; break;
                    case UnitType::VEHICLE: speed = 35.0f; break;
                    case UnitType::EQUIPMENT: speed = 15.0f; break;
                    case UnitType::SENSOR: speed = 30.0f; break;
                }
            }
            
            m_position += direction * speed * deltaTime;
            
            // Add some realistic movement variation
            m_position.x += sin(behaviorTimer * 2.0f) * 0.5f;
            m_position.z += cos(behaviorTimer * 1.5f) * 0.3f;
        } else {
            m_position = m_destination;
            m_state = UnitState::IDLE;
        }
    }
    
    // Update command feedback timer
    if (m_commandFeedbackTimer > 0.0f) {
        m_commandFeedbackTimer -= deltaTime;
    }
    
    // Simulate combat interactions
    static float combatTimer = 0.0f;
    combatTimer += deltaTime;
    if (combatTimer > 8.0f) {
        combatTimer = 0.0f;
        // Simulate taking some damage in combat scenarios
        if (rand() % 100 < 5) { // 5% chance
            TakeDamage(5.0f);
        }
    }
}

void TS::Unit::SetDestination(const glm::vec3& dest) {
    m_destination = dest;
    m_state = UnitState::MOVING;
}

void TS::Unit::TakeDamage(float damage) {
    m_health = std::max(0.0f, m_health - damage);
    if (!IsActive()) {
        std::cout << GetTypeString() << " " << m_id << " disabled!" << std::endl;
    }
}

glm::vec3 TS::Unit::GetRenderColor() const {
    if (!IsActive()) {
        return glm::vec3(0.3f, 0.3f, 0.3f); // Dark gray for disabled
    }
    
    if (m_isAllied) {
        return glm::vec3(0.1f, 0.3f, 1.0f); // Bright blue for allied
    } else {
        return glm::vec3(1.0f, 0.1f, 0.1f); // Bright red for opposition
    }
}

std::string TS::Unit::GetTypeString() const {
    switch (m_type) {
        case UnitType::PERSONNEL: return "Civilian Research";
        case UnitType::VEHICLE: return "Emergency Response";
        case UnitType::EQUIPMENT: return "Environmental Monitor";
        case UnitType::SENSOR: return "Research Sensor";
        default: return "Unknown";
    }
}

void TS::Unit::SetTargetPosition(const glm::vec3& target) {
    m_targetPosition = target;
    m_destination = target; // Update current destination
    m_state = UnitState::MOVING;
}

void TS::Unit::SetMovementSpeed(float speed) {
    m_movementSpeed = std::max(0.1f, speed); // Minimum speed of 0.1
}

void TS::Unit::SetActiveCommand(const std::string& command, float duration) {
    m_lastCommand = command;
    m_commandFeedbackTimer = duration;
    m_commandExecutionCount++;
    std::cout << "  📋 " << GetTypeString() << " " << m_id << " executing: " << command << std::endl;
}

}
