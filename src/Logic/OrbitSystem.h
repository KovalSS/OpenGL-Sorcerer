#ifndef ORBIT_SYSTEM_H
#define ORBIT_SYSTEM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct LightData {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

class OrbitSystem {
public:
    OrbitSystem();

    // Обробка всіх клавіш, що стосуються об'єктів сцени (Режими 1, 2, 3)
    void ProcessInput(GLFWwindow* window, float deltaTime);

    // --- ВОГНЯНІ КУЛІ (FIREBALLS) ---
    std::vector<glm::vec3> GetFireballPositions(float time) const;
    std::vector<glm::mat4> GetFireballMatrices(float time) const;
    LightData GetFireballLightConstant(int index, float time) const; // Отримати параметри світла для конкретної кулі

    // --- КИНДЖАЛИ (DAGGERS) ---
    std::vector<glm::mat4> GetDaggerMatrices(float time) const;

    // --- ЦЕНТРАЛЬНА ЕНЕРГЕТИЧНА СФЕРА (ORB) ---
    glm::mat4 GetEnergyOrbMatrix(float time) const;
    LightData GetEnergyOrbLight(float time) const;

private:
    // --- Налаштування Кинджалів ---
    float daggerRadius;
    float daggerSpeed;
    float daggerHeight;
    float daggerScale;

    // --- Налаштування Вогняних куль ---
    float fireballRadius;
    float fireballSpeed;
    float fireballHeight;
    float fireballScale;

    // --- Налаштування Центральної Сфери ---
    float orbHeight;
    float orbBaseScale;
    float orbIntensity;
    float orbPulseSpeed;
    float orbRotateSpeed;
    float orbPulseMin;
    float orbPulseMax;
    float orbLightPulseRatio;
    
    // Базові кольори
    glm::vec3 orbColorDiffuseBase;
    glm::vec3 orbColorAmbientBase;

    // Приватні допоміжні методи
    float calculatePulse(float time) const;
};

#endif