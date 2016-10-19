#version 330

struct DirectionalLight {
    vec3 color;
    float intensity;
    vec3 dir;
};

struct AmbientLight {
    vec3 color;
    float intensity;
};

uniform DirectionalLight dirLight;
uniform AmbientLight ambLight;

in vec3 Normal;
in vec4 Color;

out vec4 color;

void main() {
    vec3 ambience = ambLight.color * ambLight.intensity;

    vec3 directional = max(dot(-dirLight.dir, Normal), 0) * dirLight.intensity *
                       vec3(Color) * dirLight.color;
    vec3 total = ambience + directional;

    color = vec4(total, 1);
}
