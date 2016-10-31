#include "Primitives.hpp"

#define ADD_INDICES(triangle, indices) for (uint i=0; i<3; ++i) indices.push_back(triangle[i])

namespace clgl {
    namespace Primitives {
        typedef unsigned int uint;

        std::shared_ptr<Mesh> CreateBox(const glm::vec3 &halfDimensions) {
            typedef uint uint;

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
                    glm::vec4(-halfx, halfy, -halfz, 1.0f),  // 2
                    glm::vec4(-halfx, -halfy, halfz, 1.0f),  // 1

                    glm::vec4(-halfx, -halfy, halfz, 1.0f),  // 1
                    glm::vec4(-halfx, halfy, -halfz, 1.0f),  // 2
                    glm::vec4(-halfx, halfy, halfz, 1.0f),   // 3

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
                    glm::vec4(halfx, halfy, -halfz, 1.0f),  // 2
                    glm::vec4(-halfx, halfy, halfz, 1.0f),  // 1

                    glm::vec4(-halfx, halfy, halfz, 1.0f),  // 1
                    glm::vec4(halfx, halfy, -halfz, 1.0f),  // 2
                    glm::vec4(halfx, halfy, halfz, 1.0f),   // 3

                    // XY near
                    glm::vec4(-halfx, -halfy, -halfz, 1.0f), // 0
                    glm::vec4(halfx, -halfy, -halfz, 1.0f),  // 2
                    glm::vec4(-halfx, halfy, -halfz, 1.0f),  // 1

                    glm::vec4(-halfx, halfy, -halfz, 1.0f),  // 1
                    glm::vec4(halfx, -halfy, -halfz, 1.0f),  // 2
                    glm::vec4(halfx, halfy, -halfz, 1.0f),   // 3

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

        std::shared_ptr<Mesh> CreateIcosphere(float radius, uint nRefinements) {
            // First generate an icosahedron and then subdivide it to get the final sphere.
            // More specifics in generating the icosahedron can be found at:
            //  http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
            //  https://en.wikipedia.org/wiki/Regular_icosahedron
            const float gr = (1.f + std::sqrt(5.f)) * 0.5f;

            std::vector<glm::vec4> positions;

            positions.emplace_back(-1.f, gr, 0.f, 1.f);
            positions.emplace_back(1.f, gr, 0.f, 1.f);
            positions.emplace_back(-1.f, -gr, 0.f, 1.f);
            positions.emplace_back(1.f, -gr, 0.f, 1.f);

            positions.emplace_back(0.f, -1.f, gr, 1.f);
            positions.emplace_back(0.f, 1.f, gr, 1.f);
            positions.emplace_back(0.f, -1.f, -gr, 1.f);
            positions.emplace_back(0.f, 1.f, -gr, 1.f);

            positions.emplace_back(gr, 0.f, -1.f, 1.f);
            positions.emplace_back(gr, 0.f, 1.f, 1.f);
            positions.emplace_back(-gr, 0.f, -1.f, 1.f);
            positions.emplace_back(-gr, 0.f, 1.f, 1.f);

            // Force every vertex to be on the sphere of radius 'radius'
            std::for_each(positions.begin(), positions.end(),
                          [=](glm::vec4 &position) {
                              position = glm::vec4(glm::normalize(glm::vec3(position)) * radius, 1.f);
                          }
            );

            std::vector<uint> indices = {
                    0, 11, 5,
                    0, 5, 1,
                    0, 1, 7,
                    0, 7, 10,
                    0, 10, 11,

                    1, 5, 9,
                    5, 11, 4,
                    11, 10, 2,
                    10, 7, 6,
                    7, 1, 8,

                    3, 9, 4,
                    3, 4, 2,
                    3, 2, 6,
                    3, 6, 8,
                    3, 8, 9,

                    4, 9, 5,
                    2, 4, 11,
                    6, 2, 10,
                    8, 6, 7,
                    9, 8, 1
            };

            // Subdivide each triangle into 4 different triangles by adding the midpoints of on each edge. Force the one vertex to be on the sphere of the given radius.
            for (int i = 0; i < nRefinements; ++i) {
                auto totalTriangles = indices.size() / 3;

                std::vector<uint> newVertexIndices;
                newVertexIndices.reserve(totalTriangles * 4);
                for (decltype(totalTriangles) t = 0; t < totalTriangles; ++t) {
                    const int triangleOffset = t * 3;
                    const glm::vec4 midpointA = glm::vec4(glm::normalize(glm::vec3(
                            positions.at(indices[triangleOffset]) + positions.at(indices[triangleOffset + 1])) / 2.f) *
                                                          radius, 1.f);
                    const glm::vec4 midpointB = glm::vec4(glm::normalize(glm::vec3(
                            positions.at(indices[triangleOffset + 1]) + positions.at(indices[triangleOffset + 2])) /
                                                                         2.f) * radius, 1.f);
                    const glm::vec4 midpointC = glm::vec4(glm::normalize(glm::vec3(
                            positions.at(indices[triangleOffset + 2]) + positions.at(indices[triangleOffset])) / 2.f) *
                                                          radius, 1.f);

                    uint aIndex = static_cast<uint>(positions.size());
                    uint bIndex = static_cast<uint>(positions.size() + 1);
                    uint cIndex = static_cast<uint>(positions.size() + 2);

                    for (decltype(positions.size()) j = 0; j < positions.size(); ++j) {
                        if (aIndex >= positions.size() && glm::distance(midpointA, positions.at(j)) < 1e-6f) {
                            aIndex = j;
                        }

                        if (bIndex >= positions.size() && glm::distance(midpointB, positions.at(j)) < 1e-6f) {
                            bIndex = j;
                        }

                        if (cIndex >= positions.size() && glm::distance(midpointC, positions.at(j)) < 1e-6f) {
                            cIndex = j;
                        }
                    }

                    if (aIndex >= positions.size()) {
                        positions.push_back(midpointA);
                        aIndex = positions.size() - 1;
                    }

                    if (bIndex >= positions.size()) {
                        positions.push_back(midpointB);
                        bIndex = positions.size() - 1;
                    }

                    if (cIndex >= positions.size()) {
                        positions.push_back(midpointC);
                        cIndex = positions.size() - 1;
                    }

                    ADD_INDICES(glm::uvec3(indices[triangleOffset], aIndex, cIndex), newVertexIndices);
                    ADD_INDICES(glm::uvec3(indices[triangleOffset + 1], bIndex, aIndex), newVertexIndices);
                    ADD_INDICES(glm::uvec3(indices[triangleOffset + 2], cIndex, bIndex), newVertexIndices);
                    ADD_INDICES(glm::uvec3(aIndex, bIndex, cIndex), newVertexIndices);

                }
                indices = std::move(newVertexIndices);
            }

            std::vector<glm::vec4> normals(positions.size());
            for (uint i = 0; i < normals.size(); ++i) {
                normals[i] = glm::vec4(glm::normalize(glm::vec3(positions.at(i))), 0.0f);
            }

            std::vector<glm::vec2> UVs(positions.size());
            for (uint i = 0; i < UVs.size(); ++i) {
                const glm::vec3 position = glm::vec3(positions.at(i));
                float theta = acos(position.z / glm::length(position)) / M_PI;
                float phi = (atan2(position.y, position.x) / M_PI + 1.f) / 2.f;
                UVs[i] = glm::vec2(phi, theta);
            }

            std::vector<glm::vec2> texCoords(positions.size());
            std::vector<glm::vec4> colors(positions.size());
            std::fill(colors.begin(), colors.end(), glm::vec4(1.0f));

            return std::make_shared<Mesh>(
                    std::move(positions),
                    std::move(normals),
                    std::move(texCoords),
                    std::move(colors),
                    std::move(indices)
            );
        }

        std::shared_ptr<Mesh> CreatePlane(const glm::vec2 &halfDimensions, const glm::uvec2 &divisions) {
            const uint width = divisions.x;
            const uint depth = divisions.y;
            const uint numVertices = width * depth;

            std::vector<glm::vec4> positions(numVertices);
            std::vector<glm::vec4> normals(numVertices);
            std::vector<uint> indices(numVertices * 3 * 2);
            std::vector<glm::vec2> texCoords(numVertices);
            std::vector<glm::vec4> colors(numVertices);


            /////////////////
            /// POSITIONS ///
            /////////////////
            glm::vec4 position(0.0f, 0.0f, 0.0f, 1.0f);
            for (uint idx = 0; idx < width; ++idx) {
                for (uint idz = 0; idz < depth; ++idz) {
                    // x/z-coords are simply generated from for-loop indices
                    position.x = (static_cast<float>(idx) / width) * halfDimensions.x * 2;
                    position.z = (static_cast<float>(idz) / depth) * halfDimensions.y * 2;

                    positions[idx + width * idz] = position;
                }
            }

            ///////////////
            /// NORMALS ///
            ///////////////
            std::fill(normals.begin(), normals.end(), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));


            ///////////////
            /// INDICES ///
            ///////////////
            // setup vertex triangle indices
            const auto calc_flat_index = [&](uint idx, uint idy) {
                return idx + width * idy;
            };

            for (uint idx = 0; idx + 1 < width; ++idx) {
                for (uint idy = 0; idy + 1 < width; ++idy) {
                    //              0      1
                    //            3 x------x
                    //              |\     |
                    //              | \  A |
                    //              |  \   |
                    //              |   \  |
                    //              |  B \ |
                    //              |     \|
                    //            5 x------x 2
                    //                     4

                    const uint flat_index = calc_flat_index(idx, idy);

                    // triangle A
                    indices[6 * flat_index + 0] = flat_index;
                    indices[6 * flat_index + 1] = calc_flat_index(idx + 1, idy + 1);
                    indices[6 * flat_index + 2] = calc_flat_index(idx + 1, idy);

                    // triangle B
                    indices[6 * flat_index + 3] = flat_index;
                    indices[6 * flat_index + 4] = calc_flat_index(idx, idy + 1);
                    indices[6 * flat_index + 5] = calc_flat_index(idx + 1, idy + 1);
                }
            }

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