#pragma once

#include <memory>
#include <nanogui/nanogui.h>

#include "BaseScene.hpp"

namespace clgl {
    /// @brief //todo add brief description to CLGLApplication
    /// @author Benjamin Wiberg
    class Application {
    public:
        /**
         * Initializes the application.
         * @return
         */
        Application(int argc, char *argv[]);

        void run();

    private:

        /// The screen that contains the application's visuals
        const std::unique_ptr<nanogui::Screen> mScreen;

        /// The "thing" that is "happening" in the app...
        std::unique_ptr<BaseScene> mScene;
    };
}