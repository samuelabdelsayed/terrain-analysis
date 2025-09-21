#pragma once
#include <string>
#include <vector>

namespace TS {

struct ScenarioData {
    int id;
    std::string name;
    std::string description;
    int playCount;
};

class DatabaseManager {
private:
    std::vector<ScenarioData> m_scenarios;
    bool m_isInitialized;
    
public:
    DatabaseManager();
    ~DatabaseManager();
    
    bool Initialize(const std::string& dbPath);
    void Shutdown();
    
    int SaveScenario(const std::string& name, const std::string& description);
    std::vector<ScenarioData> GetAllScenarios();
    
    bool IsInitialized() const { return m_isInitialized; }
};

}
