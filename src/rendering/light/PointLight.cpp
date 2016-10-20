#include "PointLight.hpp"

namespace clgl {
    PointLight::PointLight(const glm::vec3 &color, float intensity, const Attenuation &attenuation)
            : Light(color, intensity),
              mAttenuation(attenuation) {}

    void PointLight::setUniformsInShader(std::shared_ptr<BaseShader> shader, const std::string &prefix) {
        glm::vec4 transformedLightPosition = getTransform() * glm::vec4(getPosition(), 1.0f);

        shader->uniform(prefix + "color", mColor);
        shader->uniform(prefix + "intensity", mIntensity);
        shader->uniform(prefix + "position", transformedLightPosition);
        shader->uniform(prefix + "att.a", mAttenuation.a);
        shader->uniform(prefix + "att.b", mAttenuation.b);
    }
}
