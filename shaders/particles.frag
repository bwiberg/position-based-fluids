#version 330

in vec4 velocity_;

out vec4 color;

void main() {
	color = vec4(vec3(velocity_), 1.0);
}
