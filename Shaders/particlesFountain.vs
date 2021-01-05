#version 400
layout (location = 0) in vec3 VertexInitVel; // Particle initial velocity
layout (location = 1) in float StartTime;    // Particle "birth" time

out float Transp;  // Transparency of the particle

uniform float Time;  // Animation time
uniform vec3 Gravity = vec3(0.0,-0.05,0.0);  // world coords
uniform float ParticleLifetime;  // Max particle lifetime

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Assume the initial position is (0,0,0).
    vec3 pos = vec3(0.0);
    Transp = 0.0;

    // Draw at the current position
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
