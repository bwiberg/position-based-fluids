#include "ParticleSimulationScene.hpp"

#include <iostream>
#include "util/paths.hpp"
#include "util/make_unique.hpp"

namespace pbf {
    ParticleSimulationScene::ParticleSimulationScene(cl::Context &context, cl::Device &device, cl::CommandQueue &queue)
            : BaseScene(context, device, queue),
              mShader(bwgl::Shader::create()
                              .vertex(SHADERPATH("simple.vert"))
                              .fragment(SHADERPATH("simple.frag"))
                              .build()) {

        using util::make_unique;
        using namespace bwgl;
        using namespace cl;

        const unsigned int MAX_PARTICLES = 1000;

        /// Setup particles

        // Each position must be float4 for OpenCL to work
        mPositionsGL = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        mPositionsGL->bind();
        mPositionsGL->bufferData(4 * sizeof(float) * MAX_PARTICLES, NULL);
        mPositionsGL->unbind();
        mPositionsCL = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mPositionsGL->ID());

        // Each velocity must be float4 for OpenCL to work
        mVelocitiesGL = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        mVelocitiesGL->bind();
        mVelocitiesGL->bufferData(4 * sizeof(float) * MAX_PARTICLES, NULL);
        mVelocitiesGL->unbind();
        mVelocitiesCL = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mVelocitiesGL->ID());

        mDensitiesGL = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        mDensitiesGL->bind();
        mDensitiesGL->bufferData(sizeof(float) * MAX_PARTICLES, NULL);
        mDensitiesGL->unbind();
        mDensitiesCL = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mDensitiesGL->ID());

        mParticles = make_unique<VertexArray>();
        mParticles->bind();
        mParticles->addVertexAttribute(*mPositionsGL, 3, GL_FLOAT, GL_FALSE, 0);
        mParticles->addVertexAttribute(*mVelocitiesGL, 3, GL_FLOAT, GL_FALSE, 0);
        mParticles->addVertexAttribute(*mDensitiesGL, 1, GL_FLOAT, GL_FALSE, 0);
        mParticles->unbind();

        /// Setup timestep kernel
        std::string kernelSource = "";
        if (TryReadFromFile(KERNELPATH("timestep.cl"), kernelSource)) {
            Program::Sources sources;
            sources.push_back({kernelSource.c_str(), kernelSource.length()});
            mTimestepProgram = make_unique<Program>(mContext, sources);

            cl_int flag = mTimestepProgram->build({mDevice});
            if (flag == CL_BUILD_PROGRAM_FAILURE) {
                std::cerr << "Error building: "
                          << mTimestepProgram->getBuildInfo<CL_PROGRAM_BUILD_LOG>(mDevice)
                          << std::endl;
            }

            mTimestepFunctor = make_unique<KernelFunctor>(Kernel(*mTimestepProgram, "timestep"), mQueue, NullRange, NDRange(MAX_PARTICLES), NullRange);
        }

        std::cout << "Awesome" << std::endl;
    }

    void ParticleSimulationScene::addGUI(nanogui::Screen *screen) {

    }

    void ParticleSimulationScene::reset() {

    }

    void ParticleSimulationScene::update() {
        std::cout << "update" << std::endl;
        const double currentTime = glfwGetTime();
        (*mTimestepFunctor)(*mPositionsCL, *mVelocitiesCL, mDeltaTime);
    }

    void ParticleSimulationScene::render() {

    }
}