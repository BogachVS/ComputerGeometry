#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

uniform float iTime;

void main()
{    
    FragColor = vec4(1.0) * texture(skybox, texCoords);
}