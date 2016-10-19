#pragma once

#include "rendering/BaseShader.hpp"

namespace clgl {
    class Light {
    public:
        Light(const glm::vec3 &color, float intensity);

        virtual void setUniformsInShader(std::shared_ptr<BaseShader> shader,
                                         const std::string &prefix = "") = 0;

        const glm::vec3 &getColor() const;

        float getIntensity() const;

        void setColor(const glm::vec3 &color);

        void setIntensity(float intensity);

    protected:
        glm::vec3 mColor;

        float mIntensity;
    };

    inline Light::Light(const glm::vec3 &color, float intensity)
            : mColor(color), mIntensity(intensity) {}

    inline const glm::vec3 &Light::getColor() const {
        return mColor;
    }

    inline float Light::getIntensity() const {
        return mIntensity;
    }

    inline void Light::setColor(const glm::vec3 &color) {
        mColor = color;
    }

    inline void Light::setIntensity(float intensity) {
        mIntensity = intensity;
    }
}