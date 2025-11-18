#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool useTexture;
uniform float time;

void main()
{
    // Магічні кольори для світіння
    vec3 magicColor1 = vec3(0.8, 0.2, 0.8); // Фіолетовий
    vec3 magicColor2 = vec3(0.2, 0.6, 1.0); // Блакитний
    
    // Анімація кольорів
    float colorPulse = sin(time * 2.0) * 0.5 + 0.5;
    vec3 glowColor = mix(magicColor1, magicColor2, colorPulse);
    
    // Fresnel ефект - світіння на краях
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);
    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 3.0);
    
    // Пульсуюча інтенсивність світіння
    float glowIntensity = 0.3 + 0.2 * sin(time * 3.0);
    
    // ⭐ ОСНОВНЕ ОСВІТЛЕННЯ (як у першому шейдері)
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);
    vec3 ambient = 0.3 * vec3(1.0);
    
    vec4 resultColor;
    if (useTexture) {
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        if (texColor.a < 0.1) discard;
        
        // ⭐ КОМБІНОВАНИЙ ЕФЕКТ: освітлення + світіння
        vec3 baseColor = (ambient + diffuse) * texColor.rgb; // Освітлена текстура
        vec3 emissive = glowColor * fresnel * glowIntensity; // Світіння
        
        resultColor = vec4(baseColor + emissive, texColor.a);
    } else {
        // Без текстури
        vec3 baseColor = (ambient + diffuse) * vec3(1.0); // Білий з освітленням
        vec3 emissive = glowColor * fresnel * glowIntensity; // Світіння
        
        resultColor = vec4(baseColor + emissive, 1.0);
    }
    
    FragColor = resultColor;
}