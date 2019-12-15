#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in vec3 in_normal;

out vec2 our_uv;
out vec3 our_normal;
out vec3 fragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float offsetX;
uniform vec2 scaleUV;

void main(){

	gl_Position = projection * view * model * vec4(in_position, 1.0);
	our_uv = in_uv;
	our_uv.x = our_uv.x + offsetX;
	if(scaleUV.x != 0 && scaleUV.y != 0)
		our_uv *= scaleUV;
	fragPos = vec3(model * vec4(in_position, 1.0));
	our_normal = mat3(transpose(inverse(model))) * in_normal;
}
