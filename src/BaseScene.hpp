#pragma once

#include <memory>
#include <nanogui/nanogui.h>

namespace clgl {
    /// @brief An interface to some kind of simulation scene that can be rendered.
    /// @author Benjamin Wiberg
    class BaseScene {
    public:
        /**
         * Default constructor
         * @param gui A NanoGUI Window that should contain scene-specific
         * GUI components
         */
        BaseScene(std::shared_ptr<nanogui::Window> gui);

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
         * @param dt
         */
        virtual void render(double dt) = 0;
    };
}