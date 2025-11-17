#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool useTexture;
uniform float time; // Додаємо час для анімації

void main()
{
    // Магічні кольори
    vec3 magicColor1 = vec3(0.8, 0.2, 0.8); // Фіолетовий
    vec3 magicColor2 = vec3(0.2, 0.6, 1.0); // Блакитний
    vec3 magicColor3 = vec3(0.0, 1.0, 0.8); // Бірюзовий
    
    // Анімація кольорів
    float colorPulse = sin(time * 2.0) * 0.5 + 0.5;
    vec3 glowColor = mix(magicColor1, magicColor2, colorPulse);
    
    // Fresnel ефект - світіння на краях
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);
    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 3.0);
    
    // Пульсуюча інтенсивність
    float glowIntensity = 0.3 + 0.2 * sin(time * 3.0);
    
    // Ефект рунічних символів
    float runicPattern = sin(FragPos.x * 10.0 + time * 5.0) * 
                        sin(FragPos.y * 8.0 + time * 3.0) * 
                        sin(FragPos.z * 12.0 + time * 4.0);
    runicPattern = max(0.0, runicPattern) * 0.3;
    
    vec4 resultColor;
    if (useTexture) {
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        if (texColor.a < 0.1) discard;
        
        // Додаємо світіння до текстури
        vec3 emissive = glowColor * (fresnel * glowIntensity + runicPattern);
        vec3 finalColor = texColor.rgb + emissive;
        
        resultColor = vec4(finalColor, texColor.a);
    } else {
        // Без текстури - сильне світіння
        vec3 emissive = glowColor * (fresnel * 0.8 + runicPattern);
        resultColor = vec4(emissive, 1.0);
    }
    
    FragColor = resultColor;
}