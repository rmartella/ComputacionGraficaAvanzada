#version 330 core

in vec3 our_uv;
out vec4 color;

uniform samplerCube skybox;

void main()
{
    color = texture(skybox, our_uv);
}
