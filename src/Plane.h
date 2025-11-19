#ifndef PLANE_H
#define PLANE_H

#include <glad/glad.h>
#include <vector>
#include <string>
#include "Shader.h"

class Plane {
public:
    Plane();
    Plane(const std::string& texturePath, float textureRepeat = 1.0f);
    ~Plane();
    
    void Draw(Shader& shader);
    bool loadTexture(const std::string& path);
    void setPosition(const glm::vec3& position) { this->position = position; }
    void setScale(const glm::vec3& scale) { this->scale = scale; }
    void setTextureRepeat(float repeat);
    
private:
    unsigned int planeVAO, planeVBO, planeEBO;
    unsigned int textureID;
    glm::vec3 position;
    glm::vec3 scale;
    float textureRepeat;
    
    void setupPlane();
    void cleanup();
    void updateTextureCoords();  // ← Додайте цей метод у private секцію
};

#endif