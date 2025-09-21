#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace TS {

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    
    float m_yaw;
    float m_pitch;
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;
    
    void UpdateCameraVectors();

public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 100.0f, 500.0f));
    
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float aspect) const;
    glm::vec3 GetPosition() const { return m_position; }
    glm::vec3 GetFront() const { return m_front; }
    float GetZoom() const { return m_zoom; }
    
    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    
    void SetPosition(const glm::vec3& position);
    void LookAt(const glm::vec3& target);
};

}
