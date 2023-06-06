#version 330 core
out vec4 Color;

in vec2 our_uv;

uniform sampler2D depthMap;

void main()
{             
    vec4 textureColor = texture(depthMap, our_uv);
    // Color = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    if(textureColor.a < 0.1)
        discard;
    Color = textureColor; // orthographic
}