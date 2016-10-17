#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 velocity;
layout (location = 2) in float density;

out vec3 vel;

void main() {
    vel = vec3(velocity);
	gl_Position = vec4(vec3(position), 1.0);
}
