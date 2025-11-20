#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include  <glm/glm.hpp>
#include <unordered_map>
class Shader {
public:
    Shader() : ID(0) {}
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);   
    Shader(const char* vertexPath, const char* fragmentPath);
    void use() const;    
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void checkCompileErrors(unsigned int shader, std::string type);

private:
    unsigned int ID;
    mutable std::unordered_map<std::string, int> uniformLocations; 
    int getUniformLocation(const std::string& name) const; 
};

#endif