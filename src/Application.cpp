#include "Application.hpp"

#include <iostream>
#include <util/make_unique.hpp>
#include "util/OCL_CALL.hpp"

#ifdef TARGET_OS_MAC
#include <CGLCurrent.h>
#endif

namespace clgl {
    std::map<std::string, Application::SceneCreator> Application::SceneCreators;

    Application::Application(int argc, char *argv[]) {
        // Read command line arguments
        std::vector<std::string> args;
        for (unsigned int argn = 0; argn < argc; ++argn) {
            args.push_back(std::string(argv[argn]));
        }

        if (!setupNanoGUI(args) || !setupOpenCL(args)) {
            std::exit(1);
        }

        createConfigGUI();
    }

    Application::~Application() {
        nanogui::shutdown();
    }

    void Application::addSceneCreator(std::string formattedName,
                                      SceneCreator sceneCreator) {
        SceneCreators[formattedName] = sceneCreator;
    }

    bool Application::setupOpenCL(const std::vector<std::string> args) {
        OCL_ERROR;

        int desiredPlatformIndex = -1;
        int desiredDeviceIndex = -1;

        auto iter = std::find(args.begin(), args.end(), "-cl");
        if (iter != args.end()) {
            desiredPlatformIndex = std::stoi(*(++iter));
            desiredDeviceIndex = std::stoi(*(++iter));
        }

        if (!trySelectPlatform(desiredPlatformIndex) || !trySelectDevice(desiredDeviceIndex)) {
            return false;
        }

#ifdef __linux__
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
        cl_context_properties properties[] = {
            //CL_GL_CONTEXT_KHR, (cl_context_pr
            //CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(),
            //CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
            0
        };
#elif defined _WIN32
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
        cl_context_properties properties[] = {
                CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
                CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
                CL_CONTEXT_PLATFORM, (cl_context_properties) platformIds[0],
                0
        };
#elif defined TARGET_OS_MAC
#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
        CGLContextObj glContext = CGLGetCurrentContext();
        CGLShareGroupObj shareGroup = CGLGetShareGroup(glContext);
        cl_context_properties properties[] = {
                CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
                (cl_context_properties) shareGroup,
                0};

        gcl_gl_set_sharegroup(shareGroup);
#endif

        mContext = OCL_CHECK(cl::Context({mDevice}, properties, NULL, NULL, CL_ERROR));

        //create queue to which we will push commands for the device.
        mQueue = OCL_CHECK(cl::CommandQueue(mContext, mDevice, 0, CL_ERROR));

        return true;
    }

    bool Application::setupNanoGUI(const std::vector<std::string> args) {
        nanogui::init();

        bool fullscreen = false;
        if (std::find(args.begin(), args.end(), "-f") != args.end()) {
            fullscreen = true;
        }

        Eigen::Vector2i windowSize(640, 480);
        auto iter = std::find(args.begin(), args.end(), "-w");
        if (iter != args.end()) {
            windowSize[0] = std::stoi(*(++iter));
            windowSize[1] = std::stoi(*(++iter));
        }

        mScreen = std::move(util::make_unique<Screen>(*this,
                windowSize, "CL-GL Bootstrap",
                /*resizable*/true, fullscreen, /*colorBits*/8,
                /*alphaBits*/8, /*depthBits*/24, /*stencilBits*/8,
                /*nSamples*/0));
        glfwSwapInterval(1);

        return true;
    }

    bool Application::trySelectPlatform(int commandLinePlatformIndex) {
        // Get all platforms (drivers)
        std::vector<cl::Platform> allPlatforms;
        OCL_CALL(cl::Platform::get(&allPlatforms));
        if (allPlatforms.size() == 0) {
            std::cerr << "No OpenCL platforms/drivers found. Check your OpenCL installation." << std::endl;
            return false;
        }

        // Select platform
        int platformIndex = commandLinePlatformIndex;
        if (platformIndex == -1) {
            std::cout << "Found " << allPlatforms.size() << " platforms:" << std::endl;
            for (unsigned int i = 0; i < allPlatforms.size(); ++i) {
                cl::Platform &platform = allPlatforms[i];
                std::cout << i << ": " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
            }
            std::cout << "Choose platform: ";
            std::cin >> platformIndex;
        }

        if (platformIndex < 0 || platformIndex >= allPlatforms.size()) {
            std::cerr << "Invalid platform index." << std::endl;
            return false;
        }

        mPlatform = allPlatforms[platformIndex];
        return true;
    }

    bool Application::trySelectDevice(int commandLineDeviceIndex) {
        std::vector<cl::Device> allDevices;
        OCL_CALL(mPlatform.getDevices(CL_DEVICE_TYPE_ALL, &allDevices));
        if (allDevices.size() == 0) {
            std::cerr << " No devices found. Check OpenCL installation!\n";
            return false;
        }

        // Select device of the chosen platform
        int deviceIndex = commandLineDeviceIndex;
        if (deviceIndex == -1) {
            std::cout << "Found " << allDevices.size() << " devices:" << std::endl;
            for (unsigned int i = 0; i < allDevices.size(); ++i) {
                cl::Device &device = allDevices[i];
                bool has_cl_gl_sharing =
                        device.getInfo<CL_DEVICE_EXTENSIONS>().find(GL_SHARING_EXTENSION) != std::string::npos;
                std::cout << i << ": " << device.getInfo<CL_DEVICE_NAME>()
                          << ", CL-GL interoperability: " << (has_cl_gl_sharing ? "YES" : "NO")
                          << std::endl;

            }
            std::cout << "Choose device: ";
            std::cin >> deviceIndex;
        }

        if (deviceIndex < 0 || deviceIndex >= allDevices.size()) {
            std::cerr << "Invalid device index." << std::endl;
            return false;
        }

        mDevice = allDevices[deviceIndex];
        return true;
    }

    void Application::createConfigGUI() {
        using namespace nanogui;
        Window *window = new Window(mScreen.get(), "General Controls");
        window->setPosition(Eigen::Vector2i(15, 15));
        window->setLayout(new GroupLayout());

        Widget *tools = new Widget(window);
        tools->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 4));
        Vector2i toolSize(50, 30);

        Button *play = new ToolButton(tools, ENTYPO_ICON_PLAY);
        play->setFixedSize(toolSize);
        play->setChangeCallback([=](bool state) {
            std::cout << (state ? "PLAY" : "PAUSE") << std::endl;
            this->mSceneIsPlaying = state;
        });

        Button *reset = new Button(tools, "", ENTYPO_ICON_LEVEL_UP);
        reset->setFixedSize(toolSize);
        reset->setCallback([=] {
            if (this->mSceneIsPlaying) { play->setPushed(false); this->mSceneIsPlaying = false; }
            if (this->mScene) { mScene->reset(); }
        });

        PopupButton *load = new PopupButton(tools, "LOAD");
        load->setFixedSize(toolSize);
        load->setCallback([=] {
            if (this->mSceneIsPlaying) { play->setPushed(false); }
            std::cout << "LOAD" << std::endl;
        });

        Popup *popup = load->popup();
        popup->setLayout(new GroupLayout());
        Label *label = new Label(popup, "Scenes:");

        for (auto const &entry : SceneCreators) {
            Button *sceneButton = new Button(popup, entry.first);
            sceneButton->setCallback([=, &entry] {
                load->setPushed(false);
                loadScene(entry.first);
            });
        }

        mScreen->performLayout();
    }

    void Application::loadScene(std::string formattedName) {
        auto sceneCreator = SceneCreators[formattedName];

        mScene = std::move(sceneCreator(mContext, mDevice, mQueue));
        mScene->addGUI(mScreen.get());
        mScreen->performLayout();

        mScene->reset();

        mScreen->setCaption(formattedName);
    }

    int Application::run() {
        mScreen->drawAll();
        mScreen->setVisible(true);

        nanogui::mainloop(1);

        return 0;
    }

    Application::Screen::Screen(Application &app,
                                const Eigen::Vector2i &size, const std::string &caption, bool resizable,
                                bool fullscreen, int colorBits, int alphaBits, int depthBits, int stencilBits,
                                int nSamples) : nanogui::Screen(size, caption,
                                                                resizable,
                                                                fullscreen,
                                                                colorBits, alphaBits,
                                                                depthBits,
                                                                stencilBits,
                                                                nSamples,
                                                                4, 1), mApp(app) {}

    void Application::Screen::drawContents() {
        if (!mApp.mScene) return;

        if (mApp.mSceneIsPlaying) {
            mApp.mScene->update();
        }

        mApp.mScene->render();
    }

    bool Application::Screen::keyboardEvent(int key, int scancode, int action, int modifiers) {
        if (Widget::keyboardEvent(key, scancode, action, modifiers)) { return true; }
        if (!mApp.mScene) { return false; }
        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS && !mApp.mSceneIsPlaying) {
            mApp.mScene->update();
        }
        return !mApp.mScene->keyboardEvent(key, scancode, action, modifiers);
    }

    bool Application::Screen::mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down, int modifiers) {
        if (Widget::mouseButtonEvent(p, button, down, modifiers)) { return true; }
        if (!mApp.mScene) { return false; }
        return !mApp.mScene->mouseButtonEvent(glm::ivec2(p[0], p[1]), button, down, modifiers);
    }

    bool Application::Screen::mouseMotionEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel, int button, int modifiers) {
        if (Widget::mouseMotionEvent(p, rel, button, modifiers)) { return true; }
        if (!mApp.mScene) { return false; }
        return !mApp.mScene->mouseMotionEvent(glm::ivec2(p[0], p[1]), glm::vec2(rel[0], rel[1]), button, modifiers);
    }

    bool Application::Screen::scrollEvent(const Eigen::Vector2i &p, const Eigen::Vector2f &rel) {
        if (Widget::scrollEvent(p, rel)) { return true; }
        if (!mApp.mScene) { return false; }
        return !mApp.mScene->scrollEvent(glm::ivec2(p[0], p[1]), glm::vec2(rel[0], rel[1]));
    }

    bool Application::Screen::resizeEvent(const Eigen::Vector2i &i) {
        if (!mApp.mScene) { return false; }
        return !mApp.mScene->resizeEvent(glm::ivec2(i[0], i[1]));
    }
}
