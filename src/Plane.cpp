#include "Plane.h"
#include <iostream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Plane::Plane() : textureID(0), position(0.0f), scale(1.0f), textureRepeat(1.0f) {
    setupPlane();
}

Plane::Plane(const std::string& texturePath, float textureRepeat) 
    : textureID(0), position(0.0f), scale(1.0f), textureRepeat(textureRepeat) {
    setupPlane();
    loadTexture(texturePath);
}

Plane::~Plane() {
    cleanup();
}

void Plane::setupPlane() {
    float vertices[] = {
        // positions          // texture coords
         0.5f, 0.0f,  0.5f,  textureRepeat, 0.0f,
         0.5f, 0.0f, -0.5f,  textureRepeat, textureRepeat,
        -0.5f, 0.0f, -0.5f,  0.0f, textureRepeat,
        -0.5f, 0.0f,  0.5f,  0.0f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

bool Plane::loadTexture(const std::string& path) {
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true); // Flip texture if needed
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "Texture loaded successfully: " << path << std::endl;
        return true;
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        return false;
    }
}

void Plane::Draw(Shader& shader) const{
    if (textureID == 0) {
        std::cout << "Warning: Plane texture not loaded!" << std::endl;
        return;
    }

    shader.use();
    
    // Створюємо модель матрицю з позиції та масштабу
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    
    shader.setMat4("model", model);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glBindVertexArray(planeVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Plane::cleanup() {
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &planeEBO);
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}  

void Plane::setTextureRepeat(float repeat) {
    this->textureRepeat = repeat;
    updateTextureCoords();
}

void Plane::updateTextureCoords() {
    float vertices[] = {
         0.5f, 0.0f,  0.5f,  textureRepeat, 0.0f,
         0.5f, 0.0f, -0.5f,  textureRepeat, textureRepeat,
        -0.5f, 0.0f, -0.5f,  0.0f, textureRepeat,
        -0.5f, 0.0f,  0.5f,  0.0f, 0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}
