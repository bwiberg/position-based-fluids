#include "Application.hpp"

#include <util/make_unique.hpp>
#include <iostream>

#ifdef __linux__
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined _WIN32
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined TARGET_OS_MAC
#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#endif

namespace clgl {
    Application::Application(int argc, char *argv[]) {
        std::vector<std::string> args;

        for (unsigned int argn = 0; argn < argc; ++argn) {
            args.push_back(std::string(argv[argn]));
        }

        for (auto arg : args) {
            std::cout << arg << std::endl;
        }

        int desiredPlatformIndex = -1;
        if (args.size() >= 2) {
            desiredPlatformIndex = std::stoi(args[2]);
        }

        if (!trySelectPlatform(desiredPlatformIndex)) {
            std::exit(1);
        }

        int desiredDeviceIndex = -1;
        if (args.size() >= 2) {
            desiredDeviceIndex = std::stoi(args[2]);
        }

        if (!trySelectDevice(desiredDeviceIndex)) {
            std::exit(1);
        }

        mContext = cl::Context({mDevice});

        nanogui::init();

        mScreen = util::make_unique<nanogui::Screen>(Eigen::Vector2i(500, 700), "NanoGUI test [GL 4.1]",
                /*resizable*/true, /*fullscreen*/false, /*colorBits*/8,
                /*alphaBits*/8, /*depthBits*/24, /*stencilBits*/8,
                /*nSamples*/0, /*glMajor*/4, /*glMinor*/1);
    }

    Application::~Application() {
        nanogui::shutdown();
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

    // Source: http://simpleopencl.blogspot.com/2013/06/tutorial-simple-start-with-opencl-and-c.html
    void Application::runTestKernel() {
        cl::Program::Sources sources;

        // kernel calculates for each element C=A+B
        std::string kernel_code =
                "   void kernel simple_add(global const int* A, global const int* B, global int* C){       "
                        "       C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)];                 "
                        "   }                                                                               ";
        sources.push_back({kernel_code.c_str(), kernel_code.length()});

        cl::Program program(mContext, sources);
        if (program.build({mDevice}) != CL_SUCCESS) {
            std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(mDevice) << "\n";
            exit(1);
        }


        // create buffers on the device
        cl::Buffer buffer_A(mContext, CL_MEM_READ_WRITE, sizeof(int) * 10);
        cl::Buffer buffer_B(mContext, CL_MEM_READ_WRITE, sizeof(int) * 10);
        cl::Buffer buffer_C(mContext, CL_MEM_READ_WRITE, sizeof(int) * 10);

        int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};

        //create queue to which we will push commands for the device.
        cl::CommandQueue queue(mContext, mDevice);

        //write arrays A and B to the device
        queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, sizeof(int) * 10, A);
        queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, sizeof(int) * 10, B);


        //run the kernel
        cl::KernelFunctor simple_add(cl::Kernel(program, "simple_add"), queue, cl::NullRange, cl::NDRange(10),
                                     cl::NullRange);
        simple_add(buffer_A, buffer_B, buffer_C);

        //alternative way to run the kernel
        /*cl::Kernel kernel_add=cl::Kernel(program,"simple_add");
        kernel_add.setArg(0,buffer_A);
        kernel_add.setArg(1,buffer_B);
        kernel_add.setArg(2,buffer_C);
        queue.enqueueNDRangeKernel(kernel_add,cl::NullRange,cl::NDRange(10),cl::NullRange);
        queue.finish();*/

        int C[10];
        //read result C from the device to array C
        queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, sizeof(int) * 10, C);

        std::cout << " result: \n";
        for (int i = 0; i < 10; i++) {
            std::cout << C[i] << " ";
        }
    }

    int Application::run() {
        std::cout << "Running test-application!" << std::endl;
        runTestKernel();

        mScreen->setVisible(true);
        mScreen->performLayout();

        nanogui::mainloop();

        return 0;
    }
}
