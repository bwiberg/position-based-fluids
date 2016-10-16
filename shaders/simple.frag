#version 330

in vec3 vel;

out vec4 color;

void main() {
	color = vec4(vel, 1.0);
}
