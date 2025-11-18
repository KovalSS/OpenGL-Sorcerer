#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/glad.h>
#include <vector>
#include <string>
#include "Shader.h"
class Skybox {
public:
    Skybox(const std::vector<std::string>& faces, float SIZE);
    void Draw(Shader& shader, glm::mat4 view, glm::mat4 projection);
    unsigned int getCubemapTexture() const { return cubemapTexture; }
    
private:
    float SIZE;
    unsigned int cubemapTexture;
    unsigned int skyboxVAO, skyboxVBO;
    void setupSkybox();
    unsigned int loadCubemap(const std::vector<std::string>& faces);
};

#endif