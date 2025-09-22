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
    
    // Dynamic AI-driven movement (more frequent updates)
    static float behaviorTimer = 0.0f;
    behaviorTimer += deltaTime;
    
    if (m_state == UnitState::MOVING || behaviorTimer > 2.0f) { // Reduced from 3.0f
        // AI decides new destination every 2 seconds for more activity
        if (behaviorTimer > 2.0f) {
            behaviorTimer = 0.0f;
            
            // Different behaviors based on unit type and allegiance
            if (m_isAllied) {
                // Allied units patrol in formation with VERY strict boundary constraints
                float patrolRadius = 20.0f;  // Reduced patrol radius
                float angle = (GetId() * 60.0f + behaviorTimer * 10.0f) * M_PI / 180.0f;
                glm::vec3 newDest = glm::vec3(
                    cos(angle) * patrolRadius - 15.0f,  // Much smaller offset
                    0,
                    sin(angle) * patrolRadius - 15.0f   // Much smaller offset
                );
                // VERY tight terrain boundaries (-30 to +30 maximum)
                m_destination = glm::vec3(
                    std::clamp(newDest.x, -30.0f, 30.0f),
                    0,
                    std::clamp(newDest.z, -30.0f, 30.0f)
                );
                
                // Double-check position constraint
                if (glm::length(m_destination) > 35.0f) {
                    m_destination = glm::normalize(m_destination) * 30.0f;
                }
            } else {
                // Opposition units with VERY strict boundary constraints
                float searchRadius = 25.0f;  // Much smaller radius
                float angle = (GetId() * 90.0f - behaviorTimer * 15.0f) * M_PI / 180.0f;
                glm::vec3 newDest = glm::vec3(
                    cos(angle) * searchRadius + 10.0f,  // Much smaller offset
                    0,
                    sin(angle) * searchRadius + 10.0f   // Much smaller offset
                );
                // VERY tight terrain boundaries (-30 to +30 maximum)
                m_destination = glm::vec3(
                    std::clamp(newDest.x, -30.0f, 30.0f),
                    0,
                    std::clamp(newDest.z, -30.0f, 30.0f)
                );
                
                // Double-check position constraint
                if (glm::length(m_destination) > 35.0f) {
                    m_destination = glm::normalize(m_destination) * 30.0f;
                }
            }
            m_state = UnitState::MOVING;
        }
        
        // Execute movement
        glm::vec3 direction = m_destination - m_position;
        float distance = glm::length(direction);
        
        if (distance > 2.0f) {
            direction = glm::normalize(direction);
            
            // Add movement sound effects
            static float soundTimer = 0.0f;
            soundTimer += deltaTime;
            if (soundTimer >= 3.0f) {  // Play sound every 3 seconds during movement
                if (m_isAllied) {
                    system("afplay /System/Library/Sounds/Submarine.aiff > /dev/null 2>&1 &");
                    std::cout << "🔵 Blue unit " << m_id << " maneuvering" << std::endl;
                } else {
                    system("afplay /System/Library/Sounds/Morse.aiff > /dev/null 2>&1 &");
                    std::cout << "🔴 Red unit " << m_id << " repositioning" << std::endl;
                }
                soundTimer = 0.0f;
            }
            
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
            
            // CRITICAL: Always clamp actual position to boundaries
            m_position.x = std::clamp(m_position.x, -30.0f, 30.0f);
            m_position.z = std::clamp(m_position.z, -30.0f, 30.0f);
            
            // Add some realistic movement variation
            m_position.x += sin(behaviorTimer * 2.0f) * 0.3f;
            m_position.z += cos(behaviorTimer * 1.5f) * 0.2f;
            
            // Re-clamp after movement variation
            m_position.x = std::clamp(m_position.x, -30.0f, 30.0f);
            m_position.z = std::clamp(m_position.z, -30.0f, 30.0f);
        } else {
            m_position = m_destination;
            m_state = UnitState::IDLE;
        }
    }
    
    // Update command feedback timer
    if (m_commandFeedbackTimer > 0.0f) {
        m_commandFeedbackTimer -= deltaTime;
    }
    
    // Simulate interaction scenarios
    static float interactionTimer = 0.0f;
    interactionTimer += deltaTime;
    if (interactionTimer > 8.0f) {
        interactionTimer = 0.0f;
        // Simulate taking some damage in interaction scenarios
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
    std::cout << "\a"; // Audio feedback for individual unit
}

void TS::Unit::CheckContact(const std::vector<std::unique_ptr<Unit>>& allUnits, float deltaTime) {
    if (!IsActive()) return;
    
    for (const auto& other : allUnits) {
        if (!other || !other->IsActive() || other->GetId() == m_id) continue;
        
        // Only engage with opposing teams
        if (other->IsAllied() == m_isAllied) continue;
        
        if (IsInContactRange(*other)) {
            // Contact detected - apply attrition
            float damage = deltaTime * 8.0f; // Damage per second during contact
            TakeDamage(damage);
            
            // Audio and visual feedback for contact (non-blocking)
            if (m_isAllied) {
                system("afplay /System/Library/Sounds/Ping.aiff > /dev/null 2>&1 &");
                std::cout << "🔥 Blue unit " << m_id << " in contact! Health: " 
                          << static_cast<int>(m_health) << "%" << std::endl;
            } else {
                system("afplay /System/Library/Sounds/Pop.aiff > /dev/null 2>&1 &");
                std::cout << "⚡ Red unit " << m_id << " in contact! Health: " 
                          << (int)(m_health/m_maxHealth*100) << "%" << std::endl;
            }
            
            // Reduce callsigns and activity when heavily damaged
            if (m_health < m_maxHealth * 0.3f) {
                m_movementSpeed *= 0.7f; // Slower movement when damaged
                if (m_isAllied) {
                    std::cout << "📻 Blue " << m_id << " - comms degraded, reduced activity" << std::endl;
                } else {
                    std::cout << "📻 Red " << m_id << " - effectiveness compromised" << std::endl;
                }
            }
            
            // Unit elimination (non-blocking audio)
            if (m_health <= 0) {
                system("afplay /System/Library/Sounds/Basso.aiff > /dev/null 2>&1 &");
                if (m_isAllied) {
                    std::cout << "� Blue unit " << m_id << " disabled" << std::endl;
                } else {
                    std::cout << "� Red unit " << m_id << " removed" << std::endl;
                }
            }
            break; // Only interact with one unit at a time
        }
    }
}

bool TS::Unit::IsInContactRange(const Unit& other) const {
    float contactRange = 25.0f; // Increased contact range for better detection
    float distance = glm::distance(m_position, other.GetPosition());
    return distance <= contactRange;
}

}
