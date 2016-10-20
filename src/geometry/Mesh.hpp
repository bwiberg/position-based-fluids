#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace clgl {
    struct Mesh {
        Mesh(std::vector<glm::vec4> &&positions,
             std::vector<glm::vec4> &&normals,
             std::vector<glm::vec2> &&textureCoordinates,
             std::vector<glm::vec4> &&colors,
             std::vector<unsigned int> &&indices);

        void flipNormals();

        std::vector<glm::vec4> mPositions;

        std::vector<glm::vec4> mNormals;

        std::vector<glm::vec2> mTextureCoordinates;

        std::vector<glm::vec4> mColors;

        std::vector<unsigned int> mIndices;
    };

    inline Mesh::Mesh(std::vector<glm::vec4> &&positions,
                      std::vector<glm::vec4> &&normals,
                      std::vector<glm::vec2> &&textureCoordinates,
                      std::vector<glm::vec4> &&colors,
                      std::vector<unsigned int> &&indices)
            : mPositions(positions),
              mNormals(normals),
              mTextureCoordinates(textureCoordinates),
              mColors(colors),
              mIndices(indices) {

    }

    inline void Mesh::flipNormals() {
        std::for_each(mNormals.begin(), mNormals.end(), [](glm::vec4 &normal) {
            normal = - normal;
        });
    }
}