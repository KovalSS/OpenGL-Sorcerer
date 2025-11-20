#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // Читання шейдерів з файлів
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;
    
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        
        vShaderFile.close();
        fShaderFile.close();
        
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    // Компіляція шейдерів
    unsigned int vertex, fragment;
    
    // Вершинний шейдер
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    
    // Фрагментний шейдер
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    
    // Шейдерна програма
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
    // Читання шейдерів з файлів
    std::string vertexCode, fragmentCode, geometryCode;
    std::ifstream vShaderFile, fShaderFile, gShaderFile;
    
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        gShaderFile.open(geometryPath);
        
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        
        vShaderFile.close();
        fShaderFile.close();
        gShaderFile.close();
        
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        geometryCode = gShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    
    // Компіляція шейдерів
    unsigned int vertex, fragment, geometry;
    
    // Вершинний шейдер
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    
    // Фрагментний шейдер
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    
    // Геометрі-шейдер
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    checkCompileErrors(geometry, "GEOMETRY");
    
    // Шейдерна програма
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(geometry);
}

void Shader::use() const{
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(getUniformLocation(name), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
    
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    }
    
}


int Shader::getUniformLocation(const std::string& name) const {
    // Перевіряємо, чи є локація в кеші
    if (uniformLocations.count(name)) {
        return uniformLocations.at(name);
    }
    
    // Якщо немає, отримуємо локацію через OpenGL
    int location = glGetUniformLocation(ID, name.c_str());
    
    // Кешуємо результат
    uniformLocations[name] = location; 

    if (location == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader ID " << ID << std::endl;
    }

    return location;
}
