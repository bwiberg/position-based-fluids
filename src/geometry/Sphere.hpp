#pragma once

#include <glm/glm.hpp>

namespace clgl {
    struct Sphere {
        inline Sphere(float radius = 1.0f, const glm::vec3 &position = glm::vec3(0.0f))
                        : mRadius(radius), mPosition(position) { }

        float mRadius;
        glm::vec3 mPosition;
    };
}