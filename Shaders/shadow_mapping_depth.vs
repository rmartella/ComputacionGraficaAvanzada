#version 330 core
const int MAX_BONES = 100;
layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 in_uv;
layout (location = 4) in ivec4 boneIDs;
layout (location = 5) in vec4 weights;

out vec2 our_uv;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

uniform mat4 bones[MAX_BONES];
uniform int numBones;

uniform vec2 scaleUV;
uniform vec2 offsetX;

void main()
{
	mat4 boneTransform;
	if(numBones > 0){
		boneTransform = bones[boneIDs[0]] * weights[0];
		boneTransform += bones[boneIDs[1]] * weights[1];
		boneTransform += bones[boneIDs[2]] * weights[2];
		boneTransform += bones[boneIDs[3]] * weights[3];
	}
	else
		boneTransform = mat4(1.0);

	gl_Position = lightSpaceMatrix * model * boneTransform * vec4(in_position, 1.0);
	if(scaleUV.x == 0 && scaleUV.y == 0)
		our_uv = in_uv;
	else
		our_uv = scaleUV * in_uv;
	our_uv.x += offsetX.x;
	our_uv.y += offsetX.y;
}
