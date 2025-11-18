#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    MovementSpeed = 1.0f;
    MouseSensitivity = 0.1f;
    Zoom = 45.0f;
    
    updateCameraVectors();
}
void Camera::setMovementSpeed(float speed) {
    if (speed > 0.0f) {
        MovementSpeed = speed;
        std::cout << "Camera speed set to: " << MovementSpeed << std::endl;
    }
}

void Camera::setMouseSensitivity(float sensitivity) {
    if (sensitivity > 0.0f) {
        MouseSensitivity = sensitivity;
        std::cout << "Mouse sensitivity set to: " << MouseSensitivity << std::endl;
    }
}

void Camera::setZoom(float zoom) {
    if (zoom >= 1.0f && zoom <= 90.0f) {
        Zoom = zoom;
        std::cout << "Zoom set to: " << Zoom << " degrees" << std::endl;
    }
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(int direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == 0) // FORWARD
        Position += Front * velocity;
    if (direction == 1) // BACKWARD
        Position -= Front * velocity;
    if (direction == 2) // LEFT
        Position -= Right * velocity;
    if (direction == 3) // RIGHT
        Position += Right * velocity;
    if (direction == 4) // UP
        Position += WorldUp * velocity;
    if (direction == 5) // DOWN
        Position -= WorldUp * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f) Zoom = 1.0f;
    if (Zoom > 45.0f) Zoom = 45.0f;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}