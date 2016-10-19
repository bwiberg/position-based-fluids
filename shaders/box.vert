#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec4 color;

uniform mat4 MVP;

out vec4 Color;

void main() {
    Color = normal;
	gl_Position = MVP * vec4(vec3(position), 1.0f);
}
