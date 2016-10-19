#include "AmbientLight.hpp"

namespace clgl {
    AmbientLight::AmbientLight(const glm::vec3 &color, float intensity)
            : Light(color, intensity) {}

    void AmbientLight::setUniformsInShader(std::shared_ptr<clgl::BaseShader> shader,
                                           const std::string &prefix) {
        shader->uniform(prefix + "color", mColor);
        shader->uniform(prefix + "intensity", mIntensity);
    }
}
