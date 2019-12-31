#version 330 core
const int MAX_BONES = 100;
layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in vec3 in_normal;

out vec3 our_normal;
out vec3 fragPos;
out vec4 fragPosLightSpace;
out vec2 our_uv;
out float visibility;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform mat4 lightSpaceMatrix;

uniform float density = 0.008;
uniform float gradient = 1.5;

void main(){
	vec4 fragPosWorldSpace = model * vec4(in_position, 1.0);
	gl_Position = projection * view * fragPosWorldSpace;
	vec3 fragPosViewSpace = vec3(view * fragPosWorldSpace);
	fragPos = vec3(fragPosWorldSpace);
	fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
	our_normal = mat3(transpose(inverse(model))) * in_normal;
	our_uv = in_uv;
	float distance = length(fragPosViewSpace);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}

