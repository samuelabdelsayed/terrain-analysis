#include "graphics/EntitySymbols.h"
#include <OpenGL/gl.h>
#include <cmath>

namespace TS {

void EntitySymbols::RenderUnitSymbol(const Unit& unit) {
    glm::vec3 position = unit.GetPosition();
    position.y += 50.0f; // Hover well above ground for visibility with steep terrain
    
    glm::vec3 color = unit.GetRenderColor();
    bool isAllied = unit.IsAllied();
    
    // Modify visual based on health/attrition
    float healthPercent = unit.GetHealth() / unit.GetMaxHealth();
    float symbolScale = 0.5f + (healthPercent * 0.5f);  // Smaller when damaged
    
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glScalef(symbolScale, symbolScale, symbolScale);
    
    switch (unit.GetType()) {
        case UnitType::PERSONNEL:
            RenderPersonnelSymbol(glm::vec3(0,0,0), color, isAllied, healthPercent);
            break;
        case UnitType::VEHICLE:
            RenderVehicleSymbol(glm::vec3(0,0,0), color, isAllied, healthPercent);
            break;
        case UnitType::EQUIPMENT:
            RenderEquipmentSymbol(glm::vec3(0,0,0), color, isAllied, healthPercent);
            break;
        case UnitType::SENSOR:
            RenderSensorSymbol(glm::vec3(0,0,0), color, isAllied, healthPercent);
            break;
    }
    
    glPopMatrix();
}

void EntitySymbols::RenderPersonnelSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied, float healthPercent) {
    DrawFrame(position, color, isAllied, healthPercent);
    DrawPersonnelIcon(position);
}

void EntitySymbols::RenderVehicleSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied, float healthPercent) {
    DrawFrame(position, color, isAllied, healthPercent);
    DrawVehicleIcon(position);
}

void EntitySymbols::RenderEquipmentSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied, float healthPercent) {
    DrawFrame(position, color, isAllied, healthPercent);
    DrawEquipmentIcon(position);
}

void EntitySymbols::RenderSensorSymbol(const glm::vec3& position, const glm::vec3& color, bool isAllied, float healthPercent) {
    DrawFrame(position, color, isAllied);
    DrawSensorIcon(position);
}

void EntitySymbols::DrawFrame(const glm::vec3& position, const glm::vec3& color, bool isAllied, float healthPercent) {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    
    float size = 12.0f;
    
    // Modify colors based on health
    float healthAlpha = 0.5f + (healthPercent * 0.5f);  // Fade when damaged
    float damageRed = (1.0f - healthPercent) * 0.5f;   // Add red tint when damaged
    
    if (isAllied) {
        // Allied: Rectangle with health-based appearance
        if (healthPercent < 0.3f) {
            // Heavily damaged - flashing red
            glColor3f(1.0f, 0.2f, 0.2f);
        } else if (healthPercent < 0.7f) {
            // Damaged - yellow/orange
            glColor3f(1.0f, 0.8f, 0.2f);
        } else {
            // Healthy - bright blue
            glColor3f(0.2f, 0.6f, 1.0f);
        }
        
        glLineWidth(2.0f + (healthPercent * 2.0f));  // Thinner lines when damaged
        glBegin(GL_LINE_LOOP);
        glVertex3f(-size, -size, 0);
        glVertex3f(size, -size, 0);
        glVertex3f(size, size, 0);
        glVertex3f(-size, size, 0);
        glEnd();
        
        // Inner fill with health-based transparency
        glColor4f(0.2f + damageRed, 0.6f * healthPercent, 1.0f * healthPercent, healthAlpha * 0.3f);
        glBegin(GL_QUADS);
        glVertex3f(-size*0.8f, -size*0.8f, 0);
        glVertex3f(size*0.8f, -size*0.8f, 0);
        glVertex3f(size*0.8f, size*0.8f, 0);
        glVertex3f(-size*0.8f, size*0.8f, 0);
        glEnd();
    } else {
        // Opposition: Diamond with health-based appearance
        if (healthPercent < 0.3f) {
            // Heavily damaged - dark red/black
            glColor3f(0.5f, 0.1f, 0.1f);
        } else if (healthPercent < 0.7f) {
            // Damaged - orange/yellow
            glColor3f(1.0f, 0.5f, 0.1f);
        } else {
            // Healthy - bright red
            glColor3f(1.0f, 0.2f, 0.2f);
        }
        
        glLineWidth(2.0f + (healthPercent * 2.0f));  // Thinner lines when damaged
        glBegin(GL_LINE_LOOP);
        glVertex3f(0, size, 0);
        glVertex3f(size, 0, 0);
        glVertex3f(0, -size, 0);
        glVertex3f(-size, 0, 0);
        glEnd();
        
        // Inner fill with health-based transparency
        glColor4f(1.0f * healthPercent, 0.2f + damageRed, 0.2f, healthAlpha * 0.3f);
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
