#include "DirectionalLight.hpp"

namespace clgl {
    DirectionalLight::DirectionalLight(const glm::vec3 &color,
                                       float intensity,
                                       const glm::vec3 &lightDirection,
                                       const Attenuation &att)
            : Light(color, intensity),
              mLightDirection(lightDirection),
              mAttenuation(att) {}

    void DirectionalLight::setUniformsInShader(std::shared_ptr<clgl::BaseShader> shader,
                                               const std::string &prefix) {
        glm::vec4 transformedLightDirection = getTransform() * glm::vec4(mLightDirection, 0.0f);

        shader->uniform(prefix + "color", mColor);
        shader->uniform(prefix + "intensity", mIntensity);
        shader->uniform(prefix + "dir", transformedLightDirection);
        shader->uniform(prefix + "att.a", mAttenuation.a);
        shader->uniform(prefix + "att.b", mAttenuation.b);
    }
}
