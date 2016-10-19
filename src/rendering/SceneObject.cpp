#include "SceneObject.hpp"

#include <algorithm>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pbf {
    SceneObject::SceneObject(const glm::vec3 &position, const glm::quat &orientation, float scale)
            : mPosition(position), mOrientation(orientation), mScale(scale) {}

    SceneObject::~SceneObject() {}

    void SceneObject::translate(const glm::vec3 &translation) {
        mPosition += translation;
    }

    void SceneObject::rotate(const glm::quat &rotation) {
        mOrientation = rotation * mOrientation;
    }

    void SceneObject::scale(float scale) {
        mScale *= scale;
    }

    void SceneObject::attachToParent(std::shared_ptr<SceneObject> parent) {
        detachFromParent();
        parent->mChildren.push_back(std::shared_ptr<SceneObject>(this));
        mParent = std::weak_ptr<SceneObject>(parent);
    }

    void SceneObject::detachFromParent() {
        std::shared_ptr<SceneObject> previousParent = nullptr;
        if ((previousParent = this->mParent.lock())) {
            auto &parentsChildren = previousParent->mChildren;
            auto sharedThis = std::shared_ptr<SceneObject>(this);

            parentsChildren.erase(std::remove(parentsChildren.begin(), parentsChildren.end(), sharedThis), parentsChildren.end());
        }
    }

    std::shared_ptr<const SceneObject> SceneObject::getParent() const {
        return mParent.lock();
    }

    const glm::mat4 SceneObject::getTransform() const {
        const glm::mat4 rotation = glm::toMat4(mOrientation);
        const glm::mat4 translation = glm::translate(glm::mat4(1.0f), mPosition);
        const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(mScale, mScale, mScale));

        return getParentTransform() * translation * rotation * scale;
    }

    inline glm::mat4 SceneObject::getParentTransform() const {
        auto parent = getParent();
        if (parent) {
            return parentTransform = parent->getTransform();
        }

        return glm::mat4(1.0f);
    }
}