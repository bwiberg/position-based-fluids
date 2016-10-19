#include <glm/gtc/matrix_transform.hpp>
#include "Camera.hpp"

namespace pbf {
    const float Camera::DEFAULT_Z_NEAR = 1.0f;
    const float Camera::DEFAULT_Z_FAR = 1000.0f;

    Camera::Camera(glm::uvec2 screenDimensions, float fovy,
                   const glm::vec3 &position,
                   const glm::quat &orientation,
                   float scale)
            : SceneObject(position, orientation, scale),
              mScreenDimensions(screenDimensions), mFovY(fovy),
              mZNear(DEFAULT_Z_NEAR), mZFar(DEFAULT_Z_FAR) {
        recalculatePerspectiveTransform();
    }

    void Camera::setScreenDimensions(glm::uvec2 screenDimensions) {
        mScreenDimensions = screenDimensions;
        recalculatePerspectiveTransform();
    }

    void Camera::setFieldOfViewY(float fovy) {
        mFovY = fovy;
        recalculatePerspectiveTransform();
    }

    void Camera::setClipPlanes(float zNear, float zFar) {
        mZNear = zNear;
        mZFar = zFar;
    }

    void Camera::recalculatePerspectiveTransform() {
        const float aspect = static_cast<float>(mScreenDimensions.x) / mScreenDimensions.y;
        mPerspectiveTransform = glm::perspective(mFovY, aspect, mZNear, mZFar);
    }
}
