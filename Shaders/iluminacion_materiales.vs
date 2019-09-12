#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_color;
layout (location = 3) in vec3 in_normal;

out vec3 our_color;
out vec3 our_normal;
out vec3 fragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){

	gl_Position = projection * view * model * vec4(in_position, 1.0);
	fragPos = vec3(model * vec4(in_position, 1.0));
	our_color = in_color;
	our_normal = mat3(transpose(inverse(model))) * in_normal;
}

