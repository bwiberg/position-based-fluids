#pragma once

#include <bwgl/bwgl.hpp>
#include "SceneObject.hpp"
#include "BaseShader.hpp"

namespace clgl {
    /// @brief //todo add brief description to RenderObject
    /// @author Benjamin Wiberg
    class RenderObject : public SceneObject {
    public:
        RenderObject(std::shared_ptr<BaseShader> shader);

        virtual void render(const glm::mat4 &viewProjection) = 0;

    protected:
        std::shared_ptr<BaseShader> mShader;
    };

    inline RenderObject::RenderObject(std::shared_ptr<BaseShader> shader)
            : mShader(shader) {}
}