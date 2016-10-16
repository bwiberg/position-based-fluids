#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;
layout (location = 2) in float density;

out vec3 vel;

void main() {
    vel = velocity;
	gl_Position = vec4(position, 1.0);
}
