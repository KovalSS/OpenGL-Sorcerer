// OrbitSystem.cpp
#include "OrbitSystem.h"
#include <cmath>
#include <iostream>

OrbitSystem::OrbitSystem() {
    // --- Ініціалізація Кинджалів ---
    daggerRadius = 1.8f;
    daggerSpeed = 0.5f;
    daggerHeight = 1.3f;
    daggerScale = 0.3f;

    // --- Ініціалізація Вогняних куль ---
    fireballRadius = 2.2f;
    fireballSpeed = -0.5f;
    fireballHeight = 0.8f;
    fireballScale = 0.01f;

    // --- Ініціалізація Центральної Сфери ---
    orbHeight = 3.0f;
    orbBaseScale = 0.8f;
    orbIntensity = 1.0f;
    orbPulseSpeed = 4.0f;
    orbRotateSpeed = 5.0f;
    orbPulseMin = 0.75f;
    orbPulseMax = 1.1f;
    orbLightPulseRatio = 1.5f;
    
    orbColorDiffuseBase = glm::vec3(1.5f, 1.5f, 0.0f);
    orbColorAmbientBase = glm::vec3(0.3f, 0.3f, 0.0f);
}

void OrbitSystem::ProcessInput(GLFWwindow* window, float deltaTime) {
    bool isPlus = (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS);
    bool isMinus = (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS);

    // Mode 1: Fireballs
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) {
        if (isPlus) { fireballRadius += 2.0f * deltaTime; fireballSpeed -= 1.0f * deltaTime; }
        if (isMinus) { 
            fireballRadius -= 2.0f * deltaTime; fireballSpeed += 1.0f * deltaTime; 
            if (fireballRadius < 1.5f) fireballRadius = 1.5f;
        }
    }

    // Mode 2: Daggers
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) {
        if (isPlus) { daggerRadius += 2.0f * deltaTime; daggerSpeed += 1.0f * deltaTime; }
        if (isMinus) { 
            daggerRadius -= 2.0f * deltaTime; daggerSpeed -= 1.0f * deltaTime;
            if (daggerRadius < 1.0f) daggerRadius = 1.0f;
        }
    }

    // Mode 3: Central Orb
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS) {
        if (isPlus) {
            orbHeight += 2.0f * deltaTime;
            orbBaseScale += 1.5f * deltaTime;
            orbIntensity += 1.0f * deltaTime;
        }
        if (isMinus && orbHeight > 2.5f) {
                orbHeight -= 2.0f * deltaTime;
                orbBaseScale -= 1.5f * deltaTime;
                orbIntensity -= 1.0f * deltaTime;
        }
    }
}

float OrbitSystem::calculatePulse(float time) const {
    float sineValue = sin(time * orbPulseSpeed);
    float normalizedSine = (sineValue + 1.0f) / 2.0f;
    return orbPulseMin + (normalizedSine * (orbPulseMax - orbPulseMin));
}

// --- Central Orb Logic ---
glm::mat4 OrbitSystem::GetEnergyOrbMatrix(float time) const {
    float currentPulse = calculatePulse(time);
    glm::vec3 pos(0.0f, orbHeight, 0.0f);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, time * orbRotateSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, time * orbRotateSpeed * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(orbBaseScale * currentPulse));
    
    return model;
}

LightData OrbitSystem::GetEnergyOrbLight(float time) const {
    float currentPulse = calculatePulse(time);
    float lightPulseFactor = 1.0f + ((currentPulse - 1.0f) * orbLightPulseRatio);
    if(lightPulseFactor < 0.1f) lightPulseFactor = 0.1f;

    LightData light;
    light.position = glm::vec3(0.0f, orbHeight, 0.0f);
    light.ambient = orbColorAmbientBase * orbIntensity * lightPulseFactor;
    light.diffuse = orbColorDiffuseBase * orbIntensity * lightPulseFactor;
    light.specular = glm::vec3(1.0f, 1.0f, 0.5f) * orbIntensity * lightPulseFactor;
    light.constant = 1.0f;
    light.linear = 0.14f / (orbIntensity > 0.1f ? orbIntensity : 0.1f);
    light.quadratic = 0.07f / (orbIntensity > 0.1f ? orbIntensity : 0.1f);

    return light;
}

// --- Fireball Logic ---
std::vector<glm::vec3> OrbitSystem::GetFireballPositions(float time) const {
    std::vector<glm::vec3> positions;
    float baseAngle = time * fireballSpeed;
    for (int i = 0; i < 8; i++) {
        float angle = baseAngle + glm::radians(i * 45.0f + 22.5f);
        positions.push_back(glm::vec3(cos(angle) * fireballRadius, fireballHeight, sin(angle) * fireballRadius));
    }
    return positions;
}

std::vector<glm::mat4> OrbitSystem::GetFireballMatrices(float time) const {
    std::vector<glm::vec3> positions = GetFireballPositions(time);
    std::vector<glm::mat4> matrices;

    for (const auto& pos : positions) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        
        // Billboard orientation (look at center y-axis approx)
        glm::vec3 center(0.0f, fireballHeight, 0.0f);
        glm::vec3 direction = glm::normalize(center - pos);
        glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), direction));
        glm::vec3 up = glm::normalize(glm::cross(direction, right));
        
        model[0] = glm::vec4(right, 0.0f);
        model[1] = glm::vec4(up, 0.0f);
        model[2] = glm::vec4(direction, 0.0f);

        float pulse = sin(time * 3.0f) * 0.2f + 1.0f;
        model = glm::scale(model, glm::vec3(fireballScale * pulse));
        matrices.push_back(model);
    }
    return matrices;
}

LightData OrbitSystem::GetFireballLightConstant(int index, float time) const {
    std::vector<glm::vec3> positions = GetFireballPositions(time);
    LightData light;
    if (index >= 0 && index < positions.size()) {
        light.position = positions[index];
    } else {
        light.position = glm::vec3(0.0f);
    }
    light.ambient = glm::vec3(0.05f);
    light.diffuse = glm::vec3(0.8f, 0.3f, 0.0f);
    light.specular = glm::vec3(1.0f, 0.5f, 0.0f);
    light.constant = 1.0f;
    light.linear = 0.09f;
    light.quadratic = 0.032f;
    return light;
}

// --- Dagger Logic ---
std::vector<glm::mat4> OrbitSystem::GetDaggerMatrices(float time) const {
    std::vector<glm::mat4> matrices;
    float baseAngle = time * daggerSpeed;

    for (int i = 0; i < 8; i++) {
        float angle = baseAngle + glm::radians(i * 45.0f);
        glm::vec3 pos(cos(angle) * daggerRadius, daggerHeight, sin(angle) * daggerRadius);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        
        glm::vec3 center(0.0f, daggerHeight, 0.0f);
        glm::vec3 direction = glm::normalize(center - pos);
        glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), direction));
        glm::vec3 up = glm::normalize(glm::cross(direction, right));

        model[0] = glm::vec4(right, 0.0f);
        model[1] = glm::vec4(up, 0.0f);
        model[2] = glm::vec4(direction, 0.0f);
        
        model = glm::scale(model, glm::vec3(daggerScale));
        matrices.push_back(model);
    }
    return matrices;
}