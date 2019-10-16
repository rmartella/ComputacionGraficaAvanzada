#version 330 core

in vec2 out_uv;
out vec4 color;
uniform sampler2D textura1;
uniform sampler2D textura2;

void main(){
	color = mix(texture(textura1, out_uv), texture(textura2, out_uv), 0.3);
}
