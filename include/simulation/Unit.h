#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>

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
    glm::vec3 m_targetPosition; // For operator commands
    float m_health;
    float m_maxHealth;
    float m_movementSpeed; // Configurable speed
    UnitState m_state;
    // Visual feedback for operator commands
    std::string m_lastCommand;
    float m_commandFeedbackTimer;
    int m_commandExecutionCount;
    
public:
    Unit(int id, UnitType type, const glm::vec3& position, bool isAllied);
    
    void Update(float deltaTime);
    void SetDestination(const glm::vec3& dest);
    void SetTargetPosition(const glm::vec3& target);
    void SetMovementSpeed(float speed);
    void SetActiveCommand(const std::string& command, float duration = 3.0f);
    void TakeDamage(float damage);
    void CheckEngagement(const std::vector<std::unique_ptr<Unit>>& allUnits, float deltaTime);
    bool IsInEngagementRange(const Unit& other) const;
    
    int GetId() const { return m_id; }
    UnitType GetType() const { return m_type; }
    bool IsAllied() const { return m_isAllied; }
    const glm::vec3& GetPosition() const { return m_position; }
    const glm::vec3& GetTargetPosition() const { return m_targetPosition; }
    float GetHealth() const { return m_health; }
    float GetMaxHealth() const { return m_maxHealth; }
    bool IsActive() const { return m_health > 0.0f; }
    bool HasActiveCommand() const { return m_commandFeedbackTimer > 0.0f; }
    const std::string& GetActiveCommand() const { return m_lastCommand; }
    glm::vec3 GetRenderColor() const;
    std::string GetTypeString() const;
    void SetCommand(const std::string& command) { m_lastCommand = command; }
    std::string GetCommand() const { return m_lastCommand; }
};

}
