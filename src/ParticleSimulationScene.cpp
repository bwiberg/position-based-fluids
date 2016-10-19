#include "ParticleSimulationScene.hpp"

#include "util/paths.hpp"
#include "util/OCL_CALL.hpp"
#include "util/make_unique.hpp"

#include "geometry/Primitives.hpp"

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
        mParticlesShader = make_unique<clgl::BaseShader>(
                std::unordered_map<GLuint, std::string>{{GL_VERTEX_SHADER,   SHADERPATH("particles.vert")},
                                                        {GL_FRAGMENT_SHADER, SHADERPATH("particles.frag")}});
        mParticlesShader->compile();

        mBoxShader = std::make_shared<clgl::BaseShader>(
                std::unordered_map<GLuint, std::string>{{GL_VERTEX_SHADER,   SHADERPATH("box.vert")},
                                                        {GL_FRAGMENT_SHADER, SHADERPATH("box.frag")}});
        mBoxShader->compile();

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

        /// Create camera
        mCameraRotator = std::make_shared<clgl::SceneObject>();
        mCamera = std::make_shared<clgl::Camera>(glm::uvec2(100, 100), 50);
        clgl::SceneObject::attach(mCameraRotator, mCamera);

        /// Create geometry
        auto boxMesh = clgl::Primitives::createBox(glm::vec3(1.0f, 1.0f, 1.0f));
        mBoundingBox = std::make_shared<clgl::MeshObject>(
                std::move(boxMesh),
                mBoxShader
        );

        /// Create lights
        mAmbLight = std::make_shared<clgl::AmbientLight>(glm::vec3(0.8f, 0.8f, 1.0f), 1.0f);
        mDirLight = std::make_shared<clgl::DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
    }

    void ParticleSimulationScene::addGUI(nanogui::Screen *screen) {
        auto size = screen->size();

        mCamera->setScreenDimensions(glm::uvec2(size[0], size[1]));
        mCamera->setClipPlanes(0.01f, 100.f);

        using namespace nanogui;
        Window *win = new Window(screen, "Scene Controls");
        win->setPosition(Eigen::Vector2i(15, 125));
        win->setLayout(new GroupLayout());

        Label *ambLabel = new Label(win, "Ambient Intensity");
        Slider *ambIntensity = new Slider(win);
        ambIntensity->setCallback([&](float value){
            std::cout << "Ambient Intensity: " << value << std::endl;
            mAmbLight->setIntensity(value);
        });

        Label *dirLabel = new Label(win, "Directional Intensity");
        Slider *dirIntensity = new Slider(win);
        dirIntensity->setCallback([&](float value){
            std::cout << "Directional Intensity: " << value << std::endl;
            mDirLight->setIntensity(value);
        });
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

        mCamera->setPosition(glm::vec3(0.0f, 0.0f, 15.0f));
        mDirLight->setLightDirection(glm::vec3(-1.0f));
    }

    void ParticleSimulationScene::update() {
        cl::Event event;
        OCL_CALL(mQueue.enqueueAcquireGLObjects(&mMemObjects));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mTimestepKernel, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));
        OCL_CALL(mQueue.enqueueReleaseGLObjects(&mMemObjects, NULL, &event));
        OCL_CALL(event.wait());
    }

    void ParticleSimulationScene::render() {
        OGL_CALL(glEnable(GL_DEPTH_TEST));
        OGL_CALL(glCullFace(GL_BACK));

        const glm::mat4 viewProjection = mCamera->getPerspectiveTransform() * glm::inverse(mCamera->getTransform());

        mParticlesShader->use();
        mParticlesShader->uniform("Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        mParticlesShader->uniform("MVP", viewProjection);
        mParticles->bind();
        OGL_CALL(glPointSize(5.0f));
        OGL_CALL(glDrawArrays(GL_POINTS, 0, (GLsizei) mNumParticles));
        mParticles->unbind();

        mAmbLight->setUniformsInShader(mBoxShader, "ambLight.");
        mDirLight->setUniformsInShader(mBoxShader, "dirLight.");
        mBoundingBox->render(viewProjection);
    }

    //////////////////////
    /// INPUT HANDLING ///
    //////////////////////

    bool ParticleSimulationScene::mouseButtonEvent(const glm::ivec2 &p, int button, bool down, int modifiers) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && down) {
            mIsRotatingCamera = true;
        } else if (button == GLFW_MOUSE_BUTTON_LEFT && !down) {
            mIsRotatingCamera = false;
        }

        return false;
    }

    bool
    ParticleSimulationScene::mouseMotionEvent(const glm::ivec2 &p, const glm::ivec2 &rel, int button, int modifiers) {
        if (mIsRotatingCamera) {
            std::cout << "Rotate camera, x=" << rel.x << ", y=" << rel.y << std::endl;

            glm::vec3 eulerAngles = mCameraRotator->getEulerAngles();
            eulerAngles.x += 0.05f * rel.y;
            eulerAngles.y += 0.05f * rel.x;
            mCameraRotator->setEulerAngles(eulerAngles);

            return true;
        }

        return false;
    }

    bool ParticleSimulationScene::resizeEvent(const glm::ivec2 &p) {
        std::cout << "Rezize event" << std::endl;
        return false;
    }
}