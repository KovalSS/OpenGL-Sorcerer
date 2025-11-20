#include "LightingSystem.h"

void LightingSystem::UpdateLights(Shader& shader, const OrbitSystem& orbitSystem, float time, bool enableSpecular) {
    shader.use();

    // 1. Центральна сфера (Index 0)
    LightData orbLight = orbitSystem.GetEnergyOrbLight(time);
    SetShaderLight(shader, "pointLights", 0, orbLight, enableSpecular);

    // 2. Орбітальні кулі (Index 1-8)
    for (int i = 0; i < 8; i++) {
        LightData fireballLight = orbitSystem.GetFireballLightConstant(i, time);
        SetShaderLight(shader, "pointLights", i + 1, fireballLight, enableSpecular); // Зміщення індексу на +1
    }
}

void LightingSystem::SetShaderLight(Shader& shader, const std::string& arrayName, int index, const LightData& light, bool enableSpecular) {
    std::string base = arrayName + "[" + std::to_string(index) + "]";
    
    shader.setVec3(base + ".position", light.position);
    shader.setVec3(base + ".ambient", light.ambient);
    shader.setVec3(base + ".diffuse", light.diffuse);
    
    if (enableSpecular) {
        shader.setVec3(base + ".specular", light.specular);
    } else {
        shader.setVec3(base + ".specular", glm::vec3(0.0f));
    }

    shader.setFloat(base + ".constant", light.constant);
    shader.setFloat(base + ".linear", light.linear);
    shader.setFloat(base + ".quadratic", light.quadratic);
}