/**
 * Shader adapted from NVIDIA's oclParticles demo
 * https://developer.nvidia.com/opencl#oclParticles
 * */
#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 velocity;
layout (location = 2) in float density;

out float particleDensity;
out vec3 velocity_;
out vec3 posWorld;
out vec3 posEye; // position of center in eye space

uniform mat4 MVP;
uniform mat4 MV;

uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels

void main() {
    particleDensity = density;
    velocity_ = vec3(velocity);
    posWorld = position.xyz;

    // calculate window-space point size
    posEye = vec3(MV * position);
    float dist = length(posEye);
    gl_PointSize = 1000 * pointRadius * (pointScale / dist);
    gl_PointSize = 10;

    vec3 tmp = position.xyz;
    tmp.y = -tmp.y;

    gl_Position = MVP * vec4(tmp, 1);
}
