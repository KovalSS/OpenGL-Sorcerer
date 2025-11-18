#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords; // Отримуємо текстурні координати

uniform float time;
uniform sampler2D texture_diffuse1; // Текстурний семплер

void main()
{
    // Отримуємо колір з текстури
    vec4 textureColor = texture(texture_diffuse1, TexCoords);
    
    // Яскраві кольори вогню для підсвічування
    vec3 fireColor = vec3(1.0, 0.7, 0.3);
    
    // Комбінуємо текстуру з кольором вогню
    vec3 finalColor = textureColor.rgb * fireColor;
    
    // Додаємо інтенсивність та пульсацію
    float intensity = 2.0;
    float pulse = sin(time * 4.0) * 0.2 + 0.8;
    
    // Фінальний колір
    finalColor = finalColor * intensity * pulse;
    
    FragColor = vec4(finalColor, textureColor.a);
}