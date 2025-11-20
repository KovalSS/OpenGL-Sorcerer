#ifndef LIGHTING_SYSTEM_H
#define LIGHTING_SYSTEM_H

#include "Shader.h"
#include "OrbitSystem.h"
#include <string>

class LightingSystem {
public:
    // Оновлює всі джерела світла в шейдері (центральне + 8 куль)
    static void UpdateLights(Shader& shader, const OrbitSystem& orbitSystem, float time, bool enableSpecular = true);

private:
    static void SetShaderLight(Shader& shader, const std::string& arrayName, int index, const LightData& light, bool enableSpecular);
};

#endif