#version 330

uniform vec4 Color;

in vec3 vel;

out vec4 color;

void main() {
	color = Color;
}
