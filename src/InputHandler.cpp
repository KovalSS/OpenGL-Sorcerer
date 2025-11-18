#include "InputHandler.h"
#include "Globals.h"
#include <iostream>
#include <glm/glm.hpp>

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Зберігаємо стару позицію для порівняння
    static glm::vec3 lastCameraPos = camera.Position;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(1, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(2, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(3, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(4, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(5, deltaTime);

    static bool keysProcessed[6] = {false};
    bool ctrlPressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
                       glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;

    if (ctrlPressed) {
        // Клавіші 1-3 з CTRL: чутливість повороту (миші)
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !keysProcessed[0]) {
            camera.setMouseSensitivity(0.05f);  // Низька чутливість
            std::cout << "Mouse sensitivity: LOW (0.05)" << std::endl;
            keysProcessed[0] = true;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !keysProcessed[1]) {
            camera.setMouseSensitivity(0.1f);   // Середня чутливість
            std::cout << "Mouse sensitivity: MEDIUM (0.1)" << std::endl;
            keysProcessed[1] = true;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !keysProcessed[2]) {
            camera.setMouseSensitivity(0.2f);   // Висока чутливість
            std::cout << "Mouse sensitivity: HIGH (0.2)" << std::endl;
            keysProcessed[2] = true;
        }
    } else {
        // Клавіші 1-3 без CTRL: швидкість руху (WASD)
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !keysProcessed[0]) {
            camera.setMovementSpeed(1.0f); 
            std::cout << "Movement speed:  1.0" << std::endl;
            keysProcessed[0] = true;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !keysProcessed[1]) {
            camera.setMovementSpeed(2.5f); 
            std::cout << "Movement speed:  2.5" << std::endl;
            keysProcessed[1] = true;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !keysProcessed[2]) {
            camera.setMovementSpeed(5.0f); 
            std::cout << "Movement speed:  5.0" << std::endl;
            keysProcessed[2] = true;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !keysProcessed[3]) {
        camera.setMovementSpeed(10.0f); 
        std::cout << "Movement speed:  10.0" << std::endl;
        keysProcessed[3] = true;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && !keysProcessed[4]) {
        camera.setMovementSpeed(25.0f); 
        std::cout << "Movement speed:  25.0" << std::endl;
        keysProcessed[4] = true;
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && !keysProcessed[5]) {
        camera.setMovementSpeed(50.0f); 
        std::cout << "Movement speed:  50.0" << std::endl;
        keysProcessed[5] = true;
    }
    
    // Скидаємо прапорці при відпусканні клавіш
    for (int i = 0; i < 6; i++) {
        if (glfwGetKey(window, GLFW_KEY_1 + i) == GLFW_RELEASE) {
            keysProcessed[i] = false;
        }
    }
    
    // Вивід позиції тільки при зміні
    if (glm::distance(lastCameraPos, camera.Position) > 0.01f) {
        std::cout << "Camera Position: (" 
                  << camera.Position.x << ", " 
                  << camera.Position.y << ", " 
                  << camera.Position.z << ")" 
                  << " | Zoom: " << camera.Zoom << "°"
                  << " | Speed: " << camera.getMovementSpeed() << std::endl;
        lastCameraPos = camera.Position;
    }
}