#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 in_uv;

out vec2 our_uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float scaleUV;
uniform float offsetX;

void main(){

	gl_Position = projection * view * model * vec4(in_position, 1.0);
	if(scaleUV == 0)
		our_uv = in_uv;
	else
		our_uv = scaleUV * in_uv;
	our_uv.x += offsetX;

}

