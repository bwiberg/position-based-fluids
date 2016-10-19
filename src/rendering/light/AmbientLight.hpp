#pragma once

#include "Light.hpp"

namespace clgl {
    /// @brief //todo add brief description to AmbientLight
    /// @author Benjamin Wiberg
    class AmbientLight : public Light {
    public:
        AmbientLight(const glm::vec3 &color = glm::vec3(1.0f),
                     float intensity = 1.0f);

        virtual void setUniformsInShader(std::shared_ptr<BaseShader> shader, const std::string &prefix) override;
    };
}