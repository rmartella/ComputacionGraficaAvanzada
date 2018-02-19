#version 330 core
in vec3 ex_Color;
in vec2 ex_texCoord;

out vec4 color;

uniform sampler2D textura1;
uniform sampler2D textura2;

void main()
{
	color = mix(texture(textura1, ex_texCoord), texture(textura2, ex_texCoord), 0.3);
}