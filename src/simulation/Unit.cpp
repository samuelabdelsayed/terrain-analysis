#include "simulation/Unit.h"
#include <algorithm>
#include <iostream>

namespace TS {

Unit::Unit(int id, UnitType type, const glm::vec3& position, bool isAllied)
    : m_id(id), m_type(type), m_isAllied(isAllied), m_position(position),
      m_destination(position), m_state(UnitState::IDLE) {
    
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
    
    if (m_state == UnitState::MOVING) {
        glm::vec3 direction = m_destination - m_position;
        float distance = glm::length(direction);
        
        if (distance > 1.0f) {
            direction = glm::normalize(direction);
            float speed = 50.0f; // units per second
            m_position += direction * speed * deltaTime;
        } else {
            m_position = m_destination;
            m_state = UnitState::IDLE;
        }
    }
}

void Unit::SetDestination(const glm::vec3& dest) {
    m_destination = dest;
    m_state = UnitState::MOVING;
}

void Unit::TakeDamage(float damage) {
    m_health = std::max(0.0f, m_health - damage);
    if (!IsActive()) {
        std::cout << GetTypeString() << " " << m_id << " disabled!" << std::endl;
    }
}

glm::vec3 Unit::GetRenderColor() const {
    if (!IsActive()) {
        return glm::vec3(0.3f, 0.3f, 0.3f);
    }
    
    if (m_isAllied) {
        return glm::vec3(0.0f, 0.8f, 0.0f); // Green for allied
    } else {
        return glm::vec3(0.8f, 0.0f, 0.0f); // Red for opposition
    }
}

std::string Unit::GetTypeString() const {
    switch (m_type) {
        case UnitType::PERSONNEL: return "Personnel";
        case UnitType::VEHICLE: return "Vehicle";
        case UnitType::EQUIPMENT: return "Equipment";
        case UnitType::SENSOR: return "Sensor";
        default: return "Unknown";
    }
}

}
