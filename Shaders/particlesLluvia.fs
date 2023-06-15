#version 400

uniform sampler2D ParticleTex;

in float Transp;
in vec2 TexCoord;

layout ( location = 0 ) out vec4 FragColor;

uniform vec3 colorFuego;

void main()
{
    FragColor = texture(ParticleTex, TexCoord);
    // Mix with black as it gets older, to simulate a bit of smoke
    FragColor = vec4(mix( colorFuego, FragColor.xyz, Transp ), FragColor.a);
    FragColor.a *= Transp;
}
