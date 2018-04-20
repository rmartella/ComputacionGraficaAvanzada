#version 330 core
layout (location = 0) in vec3 in_position;

out vec3 coordTexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	coordTexture = in_position;
	gl_Position = vec4(projection * view * model * vec4(in_position, 1.0)).xyww;
}