#include "core/Application.h"
#include <iostream>
#include <exception>

int main() {
    try {
        TS::Application app;
        
        std::cout << "=== Terrain Simulator ===" << std::endl;
        std::cout << "Initializing application..." << std::endl;
        
        if (!app.Initialize()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return -1;
        }
        
        std::cout << "Application initialized successfully!" << std::endl;
        std::cout << "Starting simulation..." << std::endl;
        
        app.Run();
        app.Shutdown();
        
        std::cout << "Application shutdown complete." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}
