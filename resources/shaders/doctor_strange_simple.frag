#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform bool useTexture;
uniform float time;

void main()
{
    // Інтенсивне світіння
    vec3 glowColor = vec3(0.4, 0.6, 1.0); // Сильний блакитний
    
    // Fresnel ефект
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);
    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 2.0);
    
    // Пульсація
    float pulse = 0.5 + 0.5 * sin(time * 4.0);
    
    vec4 resultColor;
    if (useTexture) {
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        if (texColor.a < 0.1) discard;
        
        // Сильне світіння поверх текстури
        vec3 emissive = glowColor * fresnel * pulse * 0.8;
        resultColor = vec4(texColor.rgb + emissive, texColor.a);
    } else {
        // Якщо нема текстури - чисте світіння
        vec3 emissive = glowColor * fresnel * pulse;
        resultColor = vec4(emissive, 1.0);
    }
    
    FragColor = resultColor;
}