#pragma once
#include <glm/glm.hpp>
#include <string>

namespace TS {

enum class UnitType {
    PERSONNEL,
    VEHICLE,
    EQUIPMENT,
    SENSOR
};

enum class UnitState {
    IDLE,
    MOVING,
    ACTIVE,
    DISABLED
};

class Unit {
private:
    int m_id;
    UnitType m_type;
    bool m_isAllied;
    glm::vec3 m_position;
    glm::vec3 m_destination;
    float m_health;
    float m_maxHealth;
    UnitState m_state;
    
public:
    Unit(int id, UnitType type, const glm::vec3& position, bool isAllied);
    
    void Update(float deltaTime);
    void SetDestination(const glm::vec3& dest);
    void TakeDamage(float damage);
    
    int GetId() const { return m_id; }
    UnitType GetType() const { return m_type; }
    bool IsAllied() const { return m_isAllied; }
    const glm::vec3& GetPosition() const { return m_position; }
    float GetHealth() const { return m_health; }
    bool IsActive() const { return m_health > 0.0f; }
    glm::vec3 GetRenderColor() const;
    std::string GetTypeString() const;
};

}
