#version 330 core
out vec4 FragColor;

float near = 0.01;
float far= 100.0;
float LinearDepth(float depth){
    float z = depth * 2.0 - 1.0; //Se regresa a NDC
    return(2.0* near * far)/(far + near - z * (far - near));
}

void main()
{
    float depthL = LinearDepth(gl_FragCoord.z) / far;
    FragColor = vec4(vec3(depthL), 1.0);	
    //FragColor = vec4(vec3(gl_FragCoord.z), 1.0);	
}