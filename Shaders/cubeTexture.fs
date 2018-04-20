#version 330 core

in vec3 coordTexture;
out vec4 color;

uniform samplerCube  textura1;

void main()
{
	color = texture(textura1, coordTexture);
    //color = vec4(1.0, 0.0, 0.0, 1.0);
}