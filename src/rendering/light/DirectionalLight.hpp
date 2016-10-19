#pragma once

#include "Light.hpp"
#include "Attenuation.hpp"
#include "rendering/SceneObject.hpp"

namespace clgl {
    /// @brief //todo add brief description to DirectionalLight
    /// @author Benjamin Wiberg
    class DirectionalLight : public SceneObject, public Light {
    public:
        DirectionalLight(const glm::vec3 &color = glm::vec3(1.0f),
                         float intensity = 1.0f,
                         const glm::vec3 &lightDirection = glm::vec3(0.0f, -1.0f, 0.0f));

        const glm::vec3 &getLightDirection() const;

        void setLightDirection(const glm::vec3 &lightDirection);

        virtual void setUniformsInShader(std::shared_ptr<BaseShader> shader,
                                         const std::string &prefix = "") override;

    private:
        glm::vec3 mLightDirection;
    };

    inline const glm::vec3 &DirectionalLight::getLightDirection() const {
        return mLightDirection;
    }

    inline void DirectionalLight::setLightDirection(const glm::vec3 &lightDirection) {
        mLightDirection = glm::normalize(lightDirection);
    }
}
