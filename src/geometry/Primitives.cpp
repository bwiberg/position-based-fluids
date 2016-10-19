#include "Primitives.hpp"

namespace clgl {
    namespace Primitives {
        std::shared_ptr<Mesh> createBox(const glm::vec3 &halfDimensions) {
            typedef unsigned int uint;

            const float halfx = halfDimensions.x;
            const float halfy = halfDimensions.y;
            const float halfz = halfDimensions.z;

            //
            // 2x------x3
            //  |\     |
            //  | \    |
            //  |  \   |
            //  |   \  |
            //  |    \ |
            //  |     \|
            // 0x------x1
            std::vector<glm::vec4> positions = {
                    // YZ left
                    glm::vec4(-halfx, -halfy, -halfz, 1.0f), // 0
                    glm::vec4(-halfx, -halfy, halfz, 1.0f),  // 1
                    glm::vec4(-halfx, halfy, -halfz, 1.0f),  // 2

                    glm::vec4(-halfx, -halfy, halfz, 1.0f),  // 1
                    glm::vec4(-halfx, halfy, halfz, 1.0f),   // 3
                    glm::vec4(-halfx, halfy, -halfz, 1.0f),  // 2

                    // YZ right
                    glm::vec4(halfx, -halfy, -halfz, 1.0f), // 0
                    glm::vec4(halfx, -halfy, halfz, 1.0f),  // 1
                    glm::vec4(halfx, halfy, -halfz, 1.0f),  // 2

                    glm::vec4(halfx, -halfy, halfz, 1.0f),  // 1
                    glm::vec4(halfx, halfy, halfz, 1.0f),   // 3
                    glm::vec4(halfx, halfy, -halfz, 1.0f),  // 2

                    // XZ bottom
                    glm::vec4(-halfx, -halfy, -halfz, 1.0f), // 0
                    glm::vec4(-halfx, -halfy, halfz, 1.0f),  // 1
                    glm::vec4(halfx, -halfy, -halfz, 1.0f),  // 2

                    glm::vec4(-halfx, -halfy, halfz, 1.0f),  // 1
                    glm::vec4(halfx, -halfy, halfz, 1.0f),   // 3
                    glm::vec4(halfx, -halfy, -halfz, 1.0f),  // 2

                    // XZ top
                    glm::vec4(-halfx, halfy, -halfz, 1.0f), // 0
                    glm::vec4(-halfx, halfy, halfz, 1.0f),  // 1
                    glm::vec4(halfx, halfy, -halfz, 1.0f),  // 2

                    glm::vec4(-halfx, halfy, halfz, 1.0f),  // 1
                    glm::vec4(halfx, halfy, halfz, 1.0f),   // 3
                    glm::vec4(halfx, halfy, -halfz, 1.0f),  // 2

                    // XY near
                    glm::vec4(-halfx, -halfy, -halfz, 1.0f), // 0
                    glm::vec4(-halfx, halfy, -halfz, 1.0f),  // 1
                    glm::vec4(halfx, -halfy, -halfz, 1.0f),  // 2

                    glm::vec4(-halfx, halfy, -halfz, 1.0f),  // 1
                    glm::vec4(halfx, halfy, -halfz, 1.0f),   // 3
                    glm::vec4(halfx, -halfy, -halfz, 1.0f),  // 2

                    // XZ far
                    glm::vec4(-halfx, -halfy, halfz, 1.0f), // 0
                    glm::vec4(-halfx, halfy, halfz, 1.0f),  // 1
                    glm::vec4(halfx, -halfy, halfz, 1.0f),  // 2

                    glm::vec4(-halfx, halfy, halfz, 1.0f),  // 1
                    glm::vec4(halfx, halfy, halfz, 1.0f),   // 3
                    glm::vec4(halfx, -halfy, halfz, 1.0f),  // 2
            };

            std::vector<glm::vec4> normals = {
                    // YZ left
                    glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
                    glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
                    glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
                    glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
                    glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),
                    glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f),

                    // YZ right
                    glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                    glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                    glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                    glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                    glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                    glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),

                    // XZ bottom
                    glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
                    glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
                    glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
                    glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
                    glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),
                    glm::vec4(0.0f, -1.0f, 0.0f, 0.0f),

                    // XZ top
                    glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                    glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                    glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                    glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                    glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                    glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),

                    // XY near
                    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
                    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
                    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
                    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
                    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),
                    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f),

                    // XY far
                    glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                    glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
            };

            std::vector<glm::vec2> texCoords(36);
            std::vector<glm::vec4> colors(36);
            std::vector<uint> indices(36);
            std::fill(colors.begin(), colors.end(), glm::vec4(1.0f));

            return std::make_shared<Mesh>(
                    std::move(positions),
                    std::move(normals),
                    std::move(texCoords),
                    std::move(colors),
                    std::move(indices)
            );
        }
    }
}