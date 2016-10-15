#include <iostream>
#include <CL/cl.hpp>
#include <nanogui/nanogui.h>
#include <glm/glm.hpp>
#include <bwgl/bwgl.hpp>

#include <tictoc.hpp>

#include <Application.hpp>

#ifdef __linux__
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined _WIN32
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined TARGET_OS_MAC
#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#endif

cl::Device chosen_device;
cl::Context context;

// Source: http://simpleopencl.blogspot.com/2013/06/tutorial-simple-start-with-opencl-and-c.html
void runTestKernel() {
    cl::Program::Sources sources;

    // kernel calculates for each element C=A+B
    std::string kernel_code =
            "   void kernel simple_add(global const int* A, global const int* B, global int* C){       "
                    "       C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)];                 "
                    "   }                                                                               ";
    sources.push_back({kernel_code.c_str(), kernel_code.length()});

    cl::Program program(context, sources);
    if (program.build({chosen_device}) != CL_SUCCESS) {
        std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(chosen_device) << "\n";
        exit(1);
    }


    // create buffers on the device
    cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
    cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int) * 10);
    cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, sizeof(int) * 10);

    int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};

    //create queue to which we will push commands for the device.
    cl::CommandQueue queue(context, chosen_device);

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

int main(int argc, char *argv[]) {
    clgl::Application app(argc, argv);
    app.run();

    // test glm
    glm::vec3 vector(1.0f, 0.0f, 0.0f);

    //get all platforms (drivers)
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if (all_platforms.size() == 0) {
        std::cout << " No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    // Select platform
    std::cout << "Found " << all_platforms.size() << " platforms:" << std::endl;
    for (unsigned int i = 0; i < all_platforms.size(); ++i) {
        cl::Platform &platform = all_platforms[i];
        std::cout << i << ": " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
    }
    std::cout << "Choice: ";
    unsigned int platform_id;
    std::cin >> platform_id;

    cl::Platform chosen_platform = all_platforms[platform_id];

    std::vector<cl::Device> all_devices;
    chosen_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if (all_devices.size() == 0) {
        std::cout << " No devices found. Check OpenCL installation!\n";
        exit(1);
    }

    // Select device of the chosen platform
    std::cout << "Found " << all_platforms.size() << " devices:" << std::endl;
    for (unsigned int i = 0; i < all_devices.size(); ++i) {
        cl::Device &device = all_devices[i];
        bool has_cl_gl_sharing = device.getInfo<CL_DEVICE_EXTENSIONS>().find(GL_SHARING_EXTENSION) != std::string::npos;
        std::cout << i << ": " << device.getInfo<CL_DEVICE_NAME>()
                  << ", CL-GL interoperability: " << (has_cl_gl_sharing ? "YES" : "NO")
                  << std::endl;

    }
    std::cout << "Choice: ";
    unsigned int device_id;
    std::cin >> device_id;

    chosen_device = all_devices[device_id];

    context = cl::Context({chosen_device});

    tic();
    runTestKernel();
    toc();

    // open nanogui window lolz
    nanogui::init();

    nanogui::Screen *screen = new nanogui::Screen(Eigen::Vector2i(500, 700), "NanoGUI test [GL 4.1]",
                /*resizable*/true, /*fullscreen*/false, /*colorBits*/8,
                /*alphaBits*/8, /*depthBits*/24, /*stencilBits*/8,
                /*nSamples*/0, /*glMajor*/4, /*glMinor*/1);

    bool enabled = true;

    screen->setVisible(true);
    screen->performLayout();

    nanogui::mainloop();

    bwgl::Texture tex(GL_TEXTURE_2D, GL_RGB, GL_UNSIGNED_BYTE);
    tex.bind();
    tex.unbind();

    nanogui::shutdown();

    return 0;
}