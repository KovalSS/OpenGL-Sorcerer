#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords; 
uniform float time;
uniform sampler2D texture_diffuse1; 

void main()
{
    vec4 textureColor = texture(texture_diffuse1, TexCoords);
    vec3 fireColor = vec3(1.0, 0.7, 0.3);
    
    vec3 finalColor = textureColor.rgb * fireColor;
    
    float intensity = 2.0;
    float pulse = sin(time * 4.0) * 0.2 + 0.8;
    
    finalColor = finalColor * intensity * pulse;
    
    FragColor = vec4(finalColor, textureColor.a);
}