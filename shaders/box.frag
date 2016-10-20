#version 330

struct Attenuation {
    float a;
    float b;
};

struct DirectionalLight {
    vec3 color;
    float intensity;
    vec3 dir;
};

struct AmbientLight {
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 color;
    float intensity;
    vec4 position;
    Attenuation att;
};

uniform DirectionalLight dirLight;
uniform AmbientLight ambLight;
uniform PointLight pointLight;

in vec4 WorldPosition;
in vec3 Normal;
in vec4 Color;

out vec4 color;

vec3 calcAmbColor() {
    return ambLight.color * ambLight.intensity;
}

vec3 calcDirColor() {
    return max(dot(-dirLight.dir, Normal), 0) * dirLight.intensity *
                                   vec3(Color) * dirLight.color;
}

vec3 calcPointColor() {
    float dist = length(WorldPosition.xyz - pointLight.position.xyz);
    vec3 dir = (pointLight.position.xyz - WorldPosition.xyz) / dist;

    vec3 clr = max(dot(dir, Normal), 0) * pointLight.intensity * vec3(Color) * pointLight.color;

    return clr / (1 + pointLight.att.a * dist + pointLight.att.b * dist * dist);
}

void main() {
    vec3 total = calcAmbColor() + calcDirColor() + calcPointColor();

    color = vec4(total, 1);
}
