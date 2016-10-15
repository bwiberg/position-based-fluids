#pragma once

#include <memory>
#include <nanogui/nanogui.h>
#include <CL/cl.hpp>

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

        ~Application();

        int run();

    private:
        bool trySelectPlatform(int commandLinePlatformIndex = -1);

        bool trySelectDevice(int commandLineDeviceIndex = -1);

        void runTestKernel();

        cl::Platform mPlatform;

        cl::Device mDevice;

        cl::Context mContext;


        /// The screen that contains the application's visuals
        std::unique_ptr<nanogui::Screen> mScreen;

        /// The "thing" that is "happening" in the app...
        std::unique_ptr<BaseScene> mScene;
    };
}