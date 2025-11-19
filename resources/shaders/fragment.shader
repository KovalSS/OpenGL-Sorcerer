#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

// Структура для точкового світла
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

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform bool useTexture;

// Масив наших вогняних куль
uniform PointLight pointLights[NR_POINT_LIGHTS];

// Функція розрахунку світла
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);

    // Проходимось по всім 8 вогняним кулям і додаємо їхнє світло
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    
    // Отримуємо колір об'єкта (текстуру)
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    if (useTexture && texColor.a < 0.1) discard;

    // Змішуємо світло з кольором текстури
    FragColor = vec4(result * texColor.rgb, texColor.a);
}

// Розрахунок для однієї кулі
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Дифузне освітлення (тіні та об'єм)
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Спекулярне освітлення (бліки)
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // 32 - це блиск (shininess)
    
    // Затухання (Attenuation) - щоб світло не світило вічно
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // Комбінуємо результати
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords)); // Фонове підсвічування
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_diffuse1, TexCoords)); // Блік бере колір текстури (або можна зробити білим)
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}