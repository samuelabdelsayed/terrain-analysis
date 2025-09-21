#include "data/DatabaseManager.h"
#include <iostream>

namespace TS {

DatabaseManager::DatabaseManager() : m_isInitialized(false) {
}

DatabaseManager::~DatabaseManager() {
    Shutdown();
}

bool DatabaseManager::Initialize(const std::string& dbPath) {
    std::cout << "Initializing Database Manager..." << std::endl;
    
    // Create sample scenarios
    SaveScenario("Training Mission", "Basic operational training");
    SaveScenario("Hill Defense", "Defend the strategic position");
    
    m_isInitialized = true;
    std::cout << "Database initialized" << std::endl;
    return true;
}

void DatabaseManager::Shutdown() {
    if (m_isInitialized) {
        std::cout << "Database Manager shutdown" << std::endl;
        m_isInitialized = false;
    }
}

int DatabaseManager::SaveScenario(const std::string& name, const std::string& description) {
    static int nextId = 1;
    ScenarioData scenario;
    scenario.id = nextId++;
    scenario.name = name;
    scenario.description = description;
    scenario.playCount = 0;
    
    m_scenarios.push_back(scenario);
    return scenario.id;
}

std::vector<ScenarioData> DatabaseManager::GetAllScenarios() {
    return m_scenarios;
}

}
