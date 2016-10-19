#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
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

        typedef std::function<std::unique_ptr<BaseScene>(cl::Context&, cl::Device&, cl::CommandQueue&)> SceneCreator;

        static void addSceneCreator(std::string formattedName, SceneCreator sceneCreator);

        int run();

    private:
        void toggleRecording(bool shouldRecord);

        void recordFrame();

        bool setupOpenCL(const std::vector<std::string> args);

        bool setupNanoGUI(const std::vector<std::string> args);

        bool trySelectPlatform(int commandLinePlatformIndex = -1);

        bool trySelectDevice(int commandLineDeviceIndex = -1);

        void createConfigGUI();

        void loadScene(std::string formattedName);

        cl::Platform mPlatform;

        cl::Device mDevice;

        cl::Context mContext;

        cl::CommandQueue mQueue;

        static std::map<std::string, SceneCreator> SceneCreators;

        /// The "thing" that is "happening" in the app...
        std::unique_ptr<BaseScene> mScene;

        bool mSceneIsPlaying;

        bool mIsRecording;

        uint mNextFrameNumber;

        class Screen : public nanogui::Screen {
        public:
            Screen(Application &app,
                   const Eigen::Vector2i &size, const std::string &caption,
                   bool resizable = true, bool fullscreen = false, int colorBits = 8,
                   int alphaBits = 8, int depthBits = 24, int stencilBits = 8,
                   int nSamples = 0);

            virtual void drawContents() override;

            virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) override;

            virtual bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down, int modifiers) override;

            virtual bool mouseMotionEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel, int button, int modifiers) override;

            virtual bool scrollEvent(const Eigen::Vector2i &p, const Eigen::Vector2f &rel) override;

            virtual bool resizeEvent(const Eigen::Vector2i &i) override;

            virtual void drawAll() override;

            virtual bool resizeEvent(const Eigen::Vector2i &i) override;

        private:
            Application &mApp;
        };

        /// The screen that contains the application's visuals
        std::unique_ptr<Screen> mScreen;
    };
}