#version 330 core
layout (location = 0) in vec3 in_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 our_uv;

void main()
{
	our_uv = in_position;
	vec4 position = projection * view * model * vec4(in_position, 1.0);
	gl_Position = position.xyww;
}
