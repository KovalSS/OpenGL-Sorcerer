#ifndef ORBIT_SYSTEM_H
#define ORBIT_SYSTEM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class OrbitSystem {
public:
    OrbitSystem();

    // Обробка натискань клавіш для зміни радіусу/швидкості
    void ProcessInput(GLFWwindow* window, float deltaTime);

    // 1. Розрахунок позицій (для джерел світла)
    std::vector<glm::vec3> GetFireballPositions(float time) const;

    // 2. Розрахунок матриць (для малювання моделей)
    std::vector<glm::mat4> GetFireballMatrices(float time, const std::vector<glm::vec3>& positions) const;
    std::vector<glm::mat4> GetDaggerMatrices(float time) const;

private:
    // Параметри Кинджалів
    float daggerRadius;
    float daggerSpeed;
    float daggerHeight;
    float daggerScale;

    // Параметри Вогняних куль
    float fireballRadius;
    float fireballSpeed;
    float fireballHeight;
    float fireballScale;

    // Допоміжна функція для створення матриці орієнтації (LookAt)
    glm::mat4 CreateOrientationMatrix(glm::vec3 position, glm::vec3 center, glm::vec3 upVector) const;
};

#endif