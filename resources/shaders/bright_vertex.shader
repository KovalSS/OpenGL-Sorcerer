#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec3 FragPos;
out vec2 TexCoords; // Додаємо вихід для текстурних координат

void main()
{
    // Анімація "дихання"
    float breath = sin(time * 2.0) * 0.1 + 1.0;
    vec3 animatedPos = aPos * breath;
    
    FragPos = vec3(model * vec4(animatedPos, 1.0));
    TexCoords = aTexCoords; // Передаємо текстурні координати
    
    gl_Position = projection * view * model * vec4(animatedPos, 1.0);
}