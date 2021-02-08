#version 330 core
const int MAX_BONES = 100;
layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 in_uv;
layout (location = 3) in vec3 in_normal;
layout (location = 4) in ivec4 boneIDs;
layout (location = 5) in vec4 weights;

out vec3 our_normal;
out vec3 fragPos;
out vec2 our_uv;
out float visibility;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform mat4 bones[MAX_BONES];
uniform int numBones;

uniform vec2 scaleUV;
uniform vec2 offsetX;

uniform float density = 0.008; 
uniform float gradient = 1.5;

void main(){

	mat4 boneTransform;
	if(numBones > 0){
		boneTransform = bones[boneIDs[0]] * weights[0];
		boneTransform += bones[boneIDs[1]] * weights[1];
		boneTransform += bones[boneIDs[2]] * weights[2];
		boneTransform += bones[boneIDs[3]] * weights[3];
	}
	else
		boneTransform = mat4(1.0);
	vec4 fragPosWorldSpace = model * boneTransform * vec4(in_position, 1.0);
	gl_Position = projection * view * fragPosWorldSpace;
	fragPos = vec3(fragPosWorldSpace);
	vec3 fragPosViewSpace = vec3 (view * fragPosWorldSpace);
	our_normal = mat3(transpose(inverse(model * boneTransform))) * in_normal;
	if(scaleUV.x == 0 && scaleUV.y == 0)
		our_uv = in_uv;
	else
		our_uv = scaleUV * in_uv;
	our_uv.x += offsetX.x;
	our_uv.y += offsetX.y;
	float distance = length (fragPosViewSpace); 
	visibility = exp(-pow((distance*density),gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}