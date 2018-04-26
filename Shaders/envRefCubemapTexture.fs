#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 color;

uniform samplerCube skybox;
uniform vec3 viewPos;

void main()
{
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(Normal));
    color = texture(skybox, R);
}