#include "MeshObject.hpp"

namespace clgl {
    MeshObject::MeshObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<BaseShader> shader, bool hasIndices)
            : RenderObject(shader),
              mNumVertices(mesh->mPositions.size()),
              mPositionsBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW),
              mNormalsBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW),
              mTexCoordsBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW),
              mColorsBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW),
              mIndicesBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW),
              mRenderElements(hasIndices),
              mNumElements(mesh->mIndices.size()) {

        mPositionsBuffer.bind();
        mPositionsBuffer.bufferData(4 * sizeof(GLfloat) * mesh->mPositions.size(),
                                    &mesh->mPositions[0]);
        mPositionsBuffer.unbind();

        mNormalsBuffer.bind();
        mNormalsBuffer.bufferData(4 * sizeof(GLfloat) * mesh->mNormals.size(),
                                  &mesh->mNormals[0]);
        mNormalsBuffer.unbind();

        mTexCoordsBuffer.bind();
        mTexCoordsBuffer.bufferData(2 * sizeof(GLfloat) * mesh->mTextureCoordinates.size(),
                                    &mesh->mTextureCoordinates[0]);
        mTexCoordsBuffer.unbind();

        mColorsBuffer.bind();
        mColorsBuffer.bufferData(3 * sizeof(GLfloat) * mesh->mColors.size(),
                                 &mesh->mColors[0]);
        mColorsBuffer.unbind();

        mIndicesBuffer.bind();
        mIndicesBuffer.bufferData(sizeof(GLuint) * mesh->mIndices.size(), &mesh->mIndices[0]);
        mIndicesBuffer.unbind();


        mVertices.bind();
        mVertices.addVertexAttribute(mPositionsBuffer, 4, GL_FLOAT, GL_FALSE, 0);
        mVertices.addVertexAttribute(mNormalsBuffer, 4, GL_FLOAT, GL_FALSE, 0);
        mVertices.addVertexAttribute(mTexCoordsBuffer, 2, GL_FLOAT, GL_FALSE, 0);
        mVertices.addVertexAttribute(mColorsBuffer, 3, GL_FLOAT, GL_FALSE, 0);
        mVertices.addElementBuffer(mIndicesBuffer);
        mVertices.unbind();
    }

    void MeshObject::render(const glm::mat4 &viewProjection) {
        const auto transform = getTransform();

        mShader->use();
        mShader->uniform("MVP", viewProjection * transform);
        mShader->uniform("M", transform);

        mVertices.bind();
        if (mRenderElements) glDrawElements(GL_TRIANGLES, mNumElements, GL_UNSIGNED_INT, 0);
        else glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
        mVertices.unbind();
    }
}
