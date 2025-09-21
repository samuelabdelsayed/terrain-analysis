#pragma once
#include <vector>
#include <memory>
#include <map>
#include "Unit.h"

namespace TS {

enum class SimulationState {
    STOPPED,
    RUNNING,
    PAUSED
};

class SimulationEngine {
private:
    // FIXED: Only store units in ONE container, not two
    std::vector<std::unique_ptr<Unit>> m_units;
    SimulationState m_state;
    float m_simulationTime;
    int m_nextUnitId;
    
public:
    SimulationEngine();
    ~SimulationEngine();
    
    void Initialize();
    void Update(float deltaTime);
    void Reset();
    
    void Start() { m_state = SimulationState::RUNNING; }
    void Pause() { m_state = SimulationState::PAUSED; }
    void Stop() { m_state = SimulationState::STOPPED; }
    
    void CreateScenario(const std::string& scenarioName);
    int AddUnit(UnitType type, const glm::vec3& position, bool isAllied = true);
    Unit* GetUnit(int unitId);
    std::vector<Unit*> GetAllUnits() const;
    
    SimulationState GetState() const { return m_state; }
    float GetSimulationTime() const { return m_simulationTime; }
    int GetUnitCount() const { return m_units.size(); }
};

}
