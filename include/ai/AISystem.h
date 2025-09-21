#pragma once

namespace TS {

class AISystem {
private:
    float m_updateTimer;
    
public:
    AISystem();
    ~AISystem();
    
    void Initialize();
    void Update(float deltaTime);
    void SetComplexity(int level);
};

}
