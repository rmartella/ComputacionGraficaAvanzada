#version 330 core

in vec3 our_uv;
out vec4 color;

uniform samplerCube skybox;
uniform float lowerLimit = 0.0;
uniform float upperLimit = 0.02;
uniform vec3 fogColor;

void main()
{
    float fogFactor = (our_uv.y - lowerLimit) / (upperLimit - lowerLimit);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    color = texture(skybox, our_uv);
    color = mix(vec4(fogColor,1.0), color, fogFactor);
}
