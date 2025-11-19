#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Структура світла (така сама, як у fragment.shader)
struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 8
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform sampler2D texture1;
uniform vec3 viewPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    // Рахуємо світло від усіх 8 куль
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    vec4 texColor = texture(texture1, TexCoords);
    FragColor = vec4(result * texColor.rgb, texColor.a);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Дифузне (тіні)
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Спекулярне (бліки) - для трави робимо їх дуже слабкими (0.1 замість 1.0)
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    
    // Затухання
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    vec3 ambient = light.ambient * vec3(texture(texture1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture1, TexCoords));
    vec3 specular = light.specular * spec * vec3(0.1); // Трава майже не блищить
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}