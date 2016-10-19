#include "ParticleSimulationScene.hpp"

#include "util/paths.hpp"
#include "util/OCL_CALL.hpp"
#include "util/make_unique.hpp"

namespace pbf {
    using util::make_unique;
    using namespace bwgl;
    using namespace cl;

    ParticleSimulationScene::ParticleSimulationScene(cl::Context &context, cl::Device &device, cl::CommandQueue &queue)
            : BaseScene(context, device, queue) {

        /// Setup particle attributes as OpenGL buffers
        mPositionsGL = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        mVelocitiesGL = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        mDensitiesGL = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);

        /// Create OpenGL vertex array, representing each particle
        mParticles = make_unique<VertexArray>();
        mParticles->bind();
        mParticles->addVertexAttribute(*mPositionsGL, 4, GL_FLOAT, GL_FALSE, /*3*sizeof(GLfloat)*/ 0);
        mParticles->addVertexAttribute(*mVelocitiesGL, 4, GL_FLOAT, GL_FALSE, /*3*sizeof(GLfloat)*/ 0);
        mParticles->addVertexAttribute(*mDensitiesGL, 1, GL_FLOAT, GL_FALSE, /*sizeof(GLfloat)*/ 0);
        mParticles->unbind();

        /// Setup shaders
        mShader = make_unique<BaseShader>(
                std::unordered_map<GLuint, std::string>{{GL_VERTEX_SHADER,   SHADERPATH("simple.vert")},
                                                        {GL_FRAGMENT_SHADER, SHADERPATH("simple.frag")}});
        mShader->compile();

        /// Setup timestep kernel
        std::string kernelSource = "";
        if (TryReadFromFile(KERNELPATH("timestep.cl"), kernelSource)) {
            cl_int error;
            mTimestepProgram = make_unique<Program>(mContext, kernelSource, true, &error);
            if (error == CL_BUILD_PROGRAM_FAILURE) {
                std::cerr << "Error building: "
                          << mTimestepProgram->getBuildInfo<CL_PROGRAM_BUILD_LOG>(mDevice)
                          << std::endl;
            }

            mTimestepKernel = make_unique<Kernel>(*mTimestepProgram, "timestep", &error);
        }

        std::cout << "Awesome" << std::endl;
    }

    void ParticleSimulationScene::addGUI(nanogui::Screen *screen) {

    }

    void ParticleSimulationScene::reset() {
        const unsigned int MAX_PARTICLES = 1000;
        mNumParticles = 1000;
        mDeltaTime = 0.001f;

        glm::vec4 positions[MAX_PARTICLES] = {glm::vec4(0)};
        glm::vec4 velocities[MAX_PARTICLES];
        {
            glm::vec4 velocity(0);
            float angle;
            for (int i = 0; i < MAX_PARTICLES; ++i) {
                angle = (float(i) / MAX_PARTICLES) * 2 * CL_M_PI_F;
                velocity.x = cosf(angle);
                velocity.y = sinf(angle);
                velocities[i] = velocity;
            }
        }

        float densities[MAX_PARTICLES] = {0};

        mPositionsGL->bind();
        mPositionsGL->bufferData(4 * sizeof(float) * MAX_PARTICLES, positions);
        mPositionsGL->unbind();

        mVelocitiesGL->bind();
        mVelocitiesGL->bufferData(4 * sizeof(float) * MAX_PARTICLES, velocities);
        mVelocitiesGL->unbind();

        mDensitiesGL->bind();
        mDensitiesGL->bufferData(sizeof(float) * MAX_PARTICLES, densities);
        mDensitiesGL->unbind();

        mPositionsCL = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mPositionsGL->ID());
        mMemObjects.push_back(*mPositionsCL);
        mVelocitiesCL = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mVelocitiesGL->ID());
        mMemObjects.push_back(*mVelocitiesCL);
        mDensitiesCL = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mDensitiesGL->ID());
        mMemObjects.push_back(*mDensitiesCL);

        OCL_CALL(mTimestepKernel->setArg(0, *mPositionsCL));
        OCL_CALL(mTimestepKernel->setArg(1, *mVelocitiesCL));
        OCL_CALL(mTimestepKernel->setArg(2, mDeltaTime));
    }

    void ParticleSimulationScene::update() {
        std::cout << "update" << std::endl;

        cl::Event event;
        OCL_CALL(mQueue.enqueueAcquireGLObjects(&mMemObjects));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mTimestepKernel, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));
        OCL_CALL(mQueue.enqueueReleaseGLObjects(&mMemObjects, NULL, &event));
        OCL_CALL(event.wait());
    }

    void ParticleSimulationScene::render() {
        std::cout << "render" << std::endl;

        mShader->use();
        mShader->uniform("Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        mParticles->bind();
        OGL_CALL(glPointSize(2.0f));
        OGL_CALL(glDrawArrays(GL_POINTS, 0, (GLsizei) mNumParticles));
        mParticles->unbind();
    }
}