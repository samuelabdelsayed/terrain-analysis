#pragma once
#include <glm/glm.hpp>
#include "simulation/Unit.h"

namespace TS {

class EntitySymbols {
public:
    static void RenderUnitSymbol(const Unit& unit);
    static void RenderPersonnelSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied);
    static void RenderVehicleSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied);
    static void RenderEquipmentSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied);
    static void RenderSensorSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied);
    
private:
    static void DrawFrame(const glm::vec3& position, const glm::vec3& color, bool isAllied);
    static void DrawPersonnelIcon(const glm::vec3& position);
    static void DrawVehicleIcon(const glm::vec3& position);
    static void DrawEquipmentIcon(const glm::vec3& position);
    static void DrawSensorIcon(const glm::vec3& position);
};

}
