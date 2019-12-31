#version 330 core

in vec2 our_uv;

uniform sampler2D texture1;

void main()
{          
	vec4 colorText = texture(texture1, our_uv);
	if(colorText.a < 0.1)
		discard;
    // gl_FragDepth = gl_FragCoord.z;
}
