#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool useTexture;

void main()
{
    // Освітлення
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);
    vec3 ambient = 0.3 * vec3(1.0);
    
    vec4 resultColor;
    if (useTexture) {
        // Текстура
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        if (texColor.a < 0.1) discard;
        resultColor = vec4((ambient + diffuse) * texColor.rgb, texColor.a);
    } else {
        // Без текстури - білий колір
        resultColor = vec4((ambient + diffuse) * vec3(1.0), 1.0);
    }
    
    FragColor = resultColor;
}