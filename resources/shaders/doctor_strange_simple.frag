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
    // Основний код з першого шейдера
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);
    vec3 ambient = 0.3 * vec3(1.0);
    
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    if (useTexture && texColor.a < 0.1) discard;
    
    vec3 baseColor = (ambient + diffuse) * texColor.rgb;
    
    // ⭐ ДОДАЄМО ПРОСТЕ СВІТІННЯ
    vec3 viewDir = normalize(viewPos - FragPos);
    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 2.0);
    float pulse = 0.3 + 0.2 * sin(time * 3.0);
    vec3 glow = vec3(0.3, 0.5, 0.8) * fresnel * pulse;
    
    vec3 resultColor = baseColor + glow;
    FragColor = vec4(resultColor, texColor.a);
}