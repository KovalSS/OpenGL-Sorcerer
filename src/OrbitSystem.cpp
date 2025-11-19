#include "OrbitSystem.h"
#include <cmath>
#include <iostream> // Для налагодження, якщо треба

OrbitSystem::OrbitSystem() {
    // Початкові налаштування (ті самі, що були в main)
    daggerRadius = 1.8f;
    daggerSpeed = 0.5f;
    daggerHeight = 1.3f;
    daggerScale = 0.3f;

    fireballRadius = 2.2f;
    fireballSpeed = -0.5f;
    fireballHeight = 0.8f;
    fireballScale = 0.01f;
}

void OrbitSystem::ProcessInput(GLFWwindow* window, float deltaTime) {
    bool isPlusPressed = (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS);
    bool isMinusPressed = (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS);

    // --- Режим 1: Вогняні Кулі ---
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) {
        if (isPlusPressed) {
            fireballRadius += 2.0f * deltaTime;
            fireballSpeed -= 1.0f * deltaTime;
        }
        if (isMinusPressed) {
            fireballRadius -= 2.0f * deltaTime;
            fireballSpeed += 1.0f * deltaTime;
            
            if (fireballRadius < 1.5f) fireballRadius = 1.5f;
            if (fireballSpeed > -0.1f) fireballSpeed = -0.1f;
        }
    }

    // --- Режим 2: Кинджали ---
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) {
        if (isPlusPressed) {
            daggerRadius += 2.0f * deltaTime;
            daggerSpeed += 1.0f * deltaTime;
        }
        if (isMinusPressed) {
            daggerRadius -= 2.0f * deltaTime;
            daggerSpeed -= 1.0f * deltaTime;

            if (daggerRadius < 1.0f) daggerRadius = 1.0f;
            if (daggerSpeed < 0.1f) daggerSpeed = 0.1f;
        }
    }
}

std::vector<glm::vec3> OrbitSystem::GetFireballPositions(float time) const {
    std::vector<glm::vec3> positions;
    float baseAngle = time * fireballSpeed;

    for (int i = 0; i < 8; i++) {
        float angle = baseAngle + glm::radians(i * 45.0f + 22.5f);
        float x = cos(angle) * fireballRadius;
        float z = sin(angle) * fireballRadius;
        positions.push_back(glm::vec3(x, fireballHeight, z));
    }
    return positions;
}

std::vector<glm::mat4> OrbitSystem::GetFireballMatrices(float time, const std::vector<glm::vec3>& positions) const {
    std::vector<glm::mat4> matrices;

    for (const auto& pos : positions) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);

        // Орієнтація
        glm::vec3 center(0.0f, fireballHeight, 0.0f);
        // Використовуємо нашу допоміжну функцію (або пишемо вручну як раніше)
        glm::vec3 direction = glm::normalize(center - pos);
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(up, direction));
        glm::vec3 actualUp = glm::normalize(glm::cross(direction, right));
        
        model[0] = glm::vec4(right, 0.0f);
        model[1] = glm::vec4(actualUp, 0.0f);
        model[2] = glm::vec4(direction, 0.0f);

        // Анімації
        model = glm::rotate(model, time * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        float pulse = sin(time * 3.0f) * 0.2f + 1.0f;
        model = glm::scale(model, glm::vec3(fireballScale * pulse));

        matrices.push_back(model);
    }
    return matrices;
}

std::vector<glm::mat4> OrbitSystem::GetDaggerMatrices(float time) const {
    std::vector<glm::mat4> matrices;
    float baseAngle = time * daggerSpeed;

    for (int i = 0; i < 8; i++) {
        float angle = baseAngle + glm::radians(i * 45.0f);
        float x = cos(angle) * daggerRadius;
        float z = sin(angle) * daggerRadius;
        glm::vec3 pos(x, daggerHeight, z);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);

        // Орієнтація
        glm::vec3 center(0.0f, daggerHeight, 0.0f);
        glm::vec3 direction = glm::normalize(center - pos);
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(up, direction));
        glm::vec3 actualUp = glm::normalize(glm::cross(direction, right));

        model[0] = glm::vec4(right, 0.0f);
        model[1] = glm::vec4(actualUp, 0.0f);
        model[2] = glm::vec4(direction, 0.0f);

        model = glm::scale(model, glm::vec3(daggerScale));
        matrices.push_back(model);
    }
    return matrices;
}