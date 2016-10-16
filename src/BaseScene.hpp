#pragma once

#include <memory>

#include <CL/cl.hpp>
#include <nanogui/nanogui.h>
#include <glm/glm.hpp>

namespace clgl {
    /// @brief An interface to some kind of simulation scene that can be rendered.
    /// @author Benjamin Wiberg
    class BaseScene {
    public:
        BaseScene(cl::Context &context, cl::CommandQueue &queue)
            : mContext(context), mQueue(queue) {
        }

        /**
         * Virtual destructor to enable proper deletion of derived classes.
         */
        virtual ~BaseScene() {}

        /**
         * Resets the scene to it's initial state.
         */
        virtual void reset() = 0;

        /**
         * Updates the scene (in some way), by taking a timestep of dt.
         * @param dt The delta time (in seconds) since last frame
         */
        virtual void update(double dt) = 0;

        /**
         * Renders the scene at its current state.
         */
        virtual void render() = 0;

        //////////////
        /// EVENTS ///
        //////////////

        virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) { return false; };

        virtual bool mouseButtonEvent(const glm::ivec2 &p, int button, bool down, int modifiers) { return false; };

        virtual bool mouseMotionEvent(const glm::ivec2 &p, const glm::ivec2 &rel, int button, int modifiers) { return false; };

        virtual bool mouseDragEvent(const glm::ivec2 &p, const glm::ivec2 &rel, int button, int modifiers) { return false; };

        virtual bool scrollEvent(const glm::ivec2 &p, const glm::vec2 &rel) { return false; };

    protected:
        cl::Context &mContext;

        cl::CommandQueue &mQueue;
    };
}