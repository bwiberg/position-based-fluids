#pragma once

#include "Mesh.hpp"

namespace clgl {
    namespace Primitives {
        std::shared_ptr<Mesh> createBox(const glm::vec3 &halfDimensions);
    }
}