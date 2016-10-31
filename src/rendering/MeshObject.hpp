#pragma once

#include "RenderObject.hpp"
#include <geometry/Mesh.hpp>

namespace clgl {
    /// @brief //todo add brief description to MeshObject
    /// @author Benjamin Wiberg
    class MeshObject : public RenderObject {
    public:
        MeshObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<BaseShader> shader, bool hasIndices = false);

        virtual void render(const glm::mat4 &viewProjection) override;

    private:
        unsigned int mNumVertices;

        unsigned int mNumElements;

        bool mRenderElements;

        bwgl::VertexArray mVertices;

        bwgl::VertexBuffer mPositionsBuffer;

        bwgl::VertexBuffer mNormalsBuffer;

        bwgl::VertexBuffer mTexCoordsBuffer;

        bwgl::VertexBuffer mColorsBuffer;

        bwgl::VertexBuffer mIndicesBuffer;
    };


}