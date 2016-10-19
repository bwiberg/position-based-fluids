#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 velocity;
layout (location = 2) in float density;

out vec4 velocity_;

uniform mat4 MVP;

void main() {
    velocity_ = velocity;
	gl_Position = MVP * vec4(vec3(position), 1.0f);
}
