#version 330 core
in vec3 ex_Color;
in vec2 ex_texCoord;

out vec4 color;

uniform sampler2D ourTexture;

void main()
{
	//color = vec4(ex_Color, 1.0f);
	//color = texture(ourTexture, ex_texCoord);
	color = texture(ourTexture, ex_texCoord) * vec4(ex_Color, 1.0f);
}