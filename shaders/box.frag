#version 330

in vec4 Color;

out vec4 color;

void main() {
	color = vec4(abs(vec3(Color)), 1);
}
