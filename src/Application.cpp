#include "Application.hpp"

#include <iostream>
#include <algorithm>
#include <util/make_unique.hpp>

#ifdef __linux__
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined _WIN32
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined TARGET_OS_MAC
#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#endif

namespace clgl {
    Application::Application(int argc, char *argv[]) {
        // Read command line arguments
        std::vector<std::string> args;
        for (unsigned int argn = 0; argn < argc; ++argn) {
            args.push_back(std::string(argv[argn]));
        }

        if (!setupOpenCL(args) || !setupNanoGUI(args)) {
            std::exit(1);
        }
    }

    Application::~Application() {
        nanogui::shutdown();
    }

    bool Application::setupOpenCL(const std::vector<std::string> args) {
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

        mContext = cl::Context({mDevice});

        //create queue to which we will push commands for the device.
        mQueue = cl::CommandQueue(mContext, mDevice);

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

        mScreen = util::make_unique<nanogui::Screen>(windowSize, "NanoGUI test [GL 4.1]",
                /*resizable*/true, fullscreen, /*colorBits*/8,
                /*alphaBits*/8, /*depthBits*/24, /*stencilBits*/8,
                /*nSamples*/0, /*glMajor*/4, /*glMinor*/1);

        return true;
    }

    bool Application::trySelectPlatform(int commandLinePlatformIndex) {
        // Get all platforms (drivers)
        std::vector<cl::Platform> allPlatforms;
        cl::Platform::get(&allPlatforms);
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
        mPlatform.getDevices(CL_DEVICE_TYPE_ALL, &allDevices);
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
                bool has_cl_gl_sharing = device.getInfo<CL_DEVICE_EXTENSIONS>().find(GL_SHARING_EXTENSION) != std::string::npos;
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

    int Application::run() {
        mScreen->setVisible(true);
        mScreen->performLayout();

        nanogui::mainloop();

        return 0;
    }
}
