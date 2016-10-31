#pragma once

#include "Mesh.hpp"

namespace clgl {
    namespace Primitives {
        std::shared_ptr<Mesh> CreateBox(const glm::vec3 &halfDimensions);

        std::shared_ptr<Mesh> CreateIcosphere(float radius, unsigned int nRefinements);

        std::shared_ptr<Mesh> CreatePlane(const glm::vec2 &halfDimensions, const glm::uvec2 &divisions);
    }
}