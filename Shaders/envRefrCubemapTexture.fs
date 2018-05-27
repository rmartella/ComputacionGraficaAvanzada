#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 color;

uniform samplerCube skybox;
uniform vec3 viewPos;

void main()
{    
    float ratio = 1.0 / 2.42;
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    color = texture(skybox, R);
}