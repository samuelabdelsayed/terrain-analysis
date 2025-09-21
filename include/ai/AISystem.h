#pragma once
#include <vector>
#include <string>

namespace TS {

class AISystem {
private:
    float m_updateTimer;
    float m_learningRate;
    int m_experience;
    std::vector<std::string> m_strategies;
    int m_currentStrategy;
    
    void LearnAndAdapt();
    void MakeTacticalDecision();
    
public:
    AISystem();
    ~AISystem();
    
    void Initialize();
    void Update(float deltaTime);
    void SetComplexity(int level);
    void ReactToPlayerCommand(const std::string& command);
};

}
