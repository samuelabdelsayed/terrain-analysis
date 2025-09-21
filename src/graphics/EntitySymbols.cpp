#include "graphics/EntitySymbols.h"
#include <OpenGL/gl.h>
#include <cmath>

namespace TS {

void EntitySymbols::RenderUnitSymbol(const Unit& unit) {
    glm::vec3 position = unit.GetPosition();
    position.y += 20.0f; // Hover above ground
    
    glm::vec3 color = unit.GetRenderColor();
    bool isAllied = unit.IsAllied();
    
    switch (unit.GetType()) {
        case UnitType::PERSONNEL:
            RenderPersonnelSymbol(position, color, isAllied);
            break;
        case UnitType::VEHICLE:
            RenderVehicleSymbol(position, color, isAllied);
            break;
        case UnitType::EQUIPMENT:
            RenderEquipmentSymbol(position, color, isAllied);
            break;
        case UnitType::SENSOR:
            RenderSensorSymbol(position, color, isAllied);
            break;
    }
}

void EntitySymbols::RenderPersonnelSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied) {
    DrawFrame(position, color, isAllied);
    DrawPersonnelIcon(position);
}

void EntitySymbols::RenderVehicleSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied) {
    DrawFrame(position, color, isAllied);
    DrawVehicleIcon(position);
}

void EntitySymbols::RenderEquipmentSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied) {
    DrawFrame(position, color, isAllied);
    DrawEquipmentIcon(position);
}

void EntitySymbols::RenderSensorSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied) {
    DrawFrame(position, color, isAllied);
    DrawSensorIcon(position);
}

void EntitySymbols::DrawFrame(const glm::vec3& position, const glm::vec3& color, bool isAllied) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    
    float size = 12.0f;
    
    if (isAllied) {
        // Allied: Rectangle with thick border (bright blue)
        glColor3f(0.2f, 0.6f, 1.0f);
        glLineWidth(4.0f);
        glBegin(GL_LINE_LOOP);
        glVertex3f(-size, -size, 0);
        glVertex3f(size, -size, 0);
        glVertex3f(size, size, 0);
        glVertex3f(-size, size, 0);
        glEnd();
        
        // Inner fill with transparency
        glColor4f(0.2f, 0.6f, 1.0f, 0.3f);
        glBegin(GL_QUADS);
        glVertex3f(-size*0.8f, -size*0.8f, 0);
        glVertex3f(size*0.8f, -size*0.8f, 0);
        glVertex3f(size*0.8f, size*0.8f, 0);
        glVertex3f(-size*0.8f, size*0.8f, 0);
        glEnd();
    } else {
        // Opposition: Diamond with thick border (bright red)
        glColor3f(1.0f, 0.2f, 0.2f);
        glLineWidth(4.0f);
        glBegin(GL_LINE_LOOP);
        glVertex3f(0, size, 0);
        glVertex3f(size, 0, 0);
        glVertex3f(0, -size, 0);
        glVertex3f(-size, 0, 0);
        glEnd();
        
        // Inner fill with transparency
        glColor4f(1.0f, 0.2f, 0.2f, 0.3f);
        glBegin(GL_QUADS);
        glVertex3f(0, size*0.8f, 0);
        glVertex3f(size*0.8f, 0, 0);
        glVertex3f(0, -size*0.8f, 0);
        glVertex3f(-size*0.8f, 0, 0);
        glEnd();
    }
    
    glPopMatrix();
}

void EntitySymbols::DrawPersonnelIcon(const glm::vec3& position) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    
    glColor3f(1.0f, 1.0f, 1.0f); // Bright white
    glLineWidth(3.0f);
    
    // Draw enhanced crossed lines (human symbol)
    glBegin(GL_LINES);
    // First line - thicker
    glVertex3f(-6, -6, 0);
    glVertex3f(6, 6, 0);
    // Second line
    glVertex3f(-6, 6, 0);
    glVertex3f(6, -6, 0);
    glEnd();
    
    // Add center dot for visibility
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex3f(0, 0, 0);
    glEnd();
    
    glPopMatrix();
}

void EntitySymbols::DrawVehicleIcon(const glm::vec3& position) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(3.0f);
    
    // Draw enhanced vehicle oval
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 20; ++i) {
        float angle = 2.0f * M_PI * i / 20.0f;
        glVertex3f(7.0f * cos(angle), 4.0f * sin(angle), 0);
    }
    glEnd();
    
    // Draw directional indicator (enhanced)
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(8, 0, 0);
    glEnd();
    
    // Add arrowhead
    glBegin(GL_LINES);
    glVertex3f(8, 0, 0);
    glVertex3f(6, 2, 0);
    glVertex3f(8, 0, 0);
    glVertex3f(6, -2, 0);
    glEnd();
    
    glPopMatrix();
}

void EntitySymbols::DrawEquipmentIcon(const glm::vec3& position) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Draw equipment symbol (simplified)
    glBegin(GL_LINE_LOOP);
    glVertex3f(-3, -3, 0);
    glVertex3f(3, -3, 0);
    glVertex3f(3, 3, 0);
    glVertex3f(-3, 3, 0);
    glEnd();
    
    // Draw extension
    glBegin(GL_LINES);
    glVertex3f(0, 3, 0);
    glVertex3f(0, 7, 0);
    glEnd();
    
    glPopMatrix();
}

void EntitySymbols::DrawSensorIcon(const glm::vec3& position) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Draw sensor symbol (circle with lines)
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 12; ++i) {
        float angle = 2.0f * M_PI * i / 12.0f;
        glVertex3f(4.0f * cos(angle), 4.0f * sin(angle), 0);
    }
    glEnd();
    
    // Draw cross inside
    glBegin(GL_LINES);
    glVertex3f(-2, 0, 0); glVertex3f(2, 0, 0);
    glVertex3f(0, -2, 0); glVertex3f(0, 2, 0);
    glEnd();
    
    glPopMatrix();
}

}
