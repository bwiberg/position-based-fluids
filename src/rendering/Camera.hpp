#pragma once

#include "SceneObject.hpp"

namespace clgl {
    /// @brief //todo add brief description to Camera
    /// @author Benjamin Wiberg
    class Camera : public SceneObject {
    public:
        static const float DEFAULT_Z_NEAR, DEFAULT_Z_FAR;

        Camera(glm::uvec2 screenDimensions, float fovy,
               const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
               const glm::quat &orientation = glm::quat(),
               float scale = 1.0f);

        void setScreenDimensions(glm::uvec2 screenDimensions);

        void setFieldOfViewY(float fovy);

        void setClipPlanes(float zNear, float zFar);

        inline const glm::mat4 &getPerspectiveTransform() const {
            return mPerspectiveTransform;
        }

        inline float getFieldOfViewY() const {
            return mFovY;
        }

        inline const glm::uvec2 &getScreenDimensions() const {
            return mScreenDimensions;
        }

    private:
        void recalculatePerspectiveTransform();

        float mZNear, mZFar;

        glm::uvec2 mScreenDimensions;

        float mFovY;

        glm::mat4 mPerspectiveTransform;
    };
}