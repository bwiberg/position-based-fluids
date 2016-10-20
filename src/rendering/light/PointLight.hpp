#pragma once

#include "Light.hpp"
#include "Attenuation.hpp"
#include "rendering/SceneObject.hpp"

namespace clgl {
    /// @brief //todo add brief description to PointLight
    /// @author Benjamin Wiberg
    class PointLight : public Light, public SceneObject {
    public:
        PointLight(const glm::vec3 &color = glm::vec3(1.0f),
                   float intensity = 1.0f,
                   const Attenuation &attenuation = Attenuation());

        const Attenuation &getAttenuation() const;

        void setAttenuation(const Attenuation &attenuation);

        virtual void setUniformsInShader(std::shared_ptr<BaseShader> shader, const std::string &prefix) override;

    private:
        Attenuation mAttenuation;
    };

    inline const Attenuation &PointLight::getAttenuation() const {
        return mAttenuation;
    }

    inline void PointLight::setAttenuation(const Attenuation &attenuation) {
        mAttenuation = attenuation;
    }
}