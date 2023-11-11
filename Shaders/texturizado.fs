#version 330 core

in vec2 our_uv;
out vec4 color;
uniform sampler2D outTexture;

void main(){
	color = texture(outTexture, our_uv);
}