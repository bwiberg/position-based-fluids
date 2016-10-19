#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace clgl {
    /// @brief //todo add brief description to SceneObject
    /// @author Benjamin Wiberg
    class SceneObject {
    public:
        SceneObject(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
                    const glm::quat &orientation = glm::quat(),
                    float scale = 1.0f);

        virtual ~SceneObject();

        /**
         * Translates this SceneObject in its local frame.
         * @param translation The translation vector
         */
        void translate(const glm::vec3 &translation);

        /**
         * Rotates this SceneObject in its local frame.
         * @param rotation The rotation quaternion
         */
        void rotate(const glm::quat &rotation);

        /**
         * Scales this SceneObject uniformly.
         * @param scale The scale factor
         */
        void scale(float scale);

        /**
         * Sets this SceneObject's position in its local frame.
         * @param position The new position
         */
        inline void setPosition(const glm::vec3 &position) {
            mPosition = position;
        }

        /**
         * Sets this SceneObject's orientation in its local fram.
         * @param orientation The new orientation
         */
        inline void setOrientation(const glm::quat &orientation) {
            mOrientation = orientation;
            recalcEulerFromQuat();
        }

        inline void setEulerAngles(const glm::vec3 &eulerAngles) {
            mEulerAngles = eulerAngles;
            recalcQuatFromEuler();
        }

        inline void setScale(float scale) {
            mScale = scale;
        }

        /**
         * Attaches a SceneObject to another SceneObject, effectively shifting the child's SceneObject's frame.
         * @param parent The new parent
         */
        static void attach(std::shared_ptr<SceneObject> parent, std::shared_ptr<SceneObject> child);

        /**
         * Detaches this SceneObject from its parent (if it has one).
         */
        void detachFromParent();

        /**
         * Gets this SceneObject's parent (if it has one).
         * @return The parent, or nullptr if it does not have one
         */
        std::shared_ptr<const SceneObject> getParent() const;

        inline const glm::vec3 &getPosition() const {
            return mPosition;
        }

        inline const glm::quat &getOrientation() const {
            return mOrientation;
        }

        inline const glm::vec3 &getEulerAngles() const {
            return mEulerAngles;
        }

        inline float getScale() const {
            return mScale;
        }

        const glm::mat4 getTransform() const;

    protected:
        glm::vec3 mPosition;

        glm::quat mOrientation;

        glm::vec3 mEulerAngles;

        float mScale;

        std::vector<std::shared_ptr<SceneObject>> mChildren;

        std::weak_ptr<SceneObject> mParent;

    private:
        glm::mat4 getParentTransform() const;

        void recalcEulerFromQuat();

        void recalcQuatFromEuler();
    };
}