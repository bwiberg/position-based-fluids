#include <OpenCL/opencl.h>
#include "ParticleSimulationScene.hpp"

#include "util/paths.hpp"
#include "util/OCL_CALL.hpp"
#include "util/make_unique.hpp"
#include "util/math_util.hpp"

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

        /// Setup "clip to bounds"-kernel
        if (TryReadFromFile(KERNELPATH("clip_to_bounds.cl"), kernelSource)) {
            cl_int error;
            mClipToBoundsProgram = make_unique<Program>(mContext, kernelSource, true, &error);
            if (error == CL_BUILD_PROGRAM_FAILURE) {
                std::cerr << "Error building: "
                          << mClipToBoundsProgram->getBuildInfo<CL_PROGRAM_BUILD_LOG>(mDevice)
                          << std::endl;
            }

            mClipToBoundsKernel = make_unique<Kernel>(*mClipToBoundsProgram, "clip_to_bounds", &error);
        }

        std::cout << "Awesome" << std::endl;

        /// Create camera
        mCameraRotator = std::make_shared<clgl::SceneObject>();
        mCamera = std::make_shared<clgl::Camera>(glm::uvec2(100, 100), 50);
        clgl::SceneObject::attach(mCameraRotator, mCamera);

        /// Create geometry
        auto boxMesh = clgl::Primitives::createBox(glm::vec3(1.0f, 1.0f, 1.0f));
        boxMesh->flipNormals();
        mBoundingBox = std::make_shared<clgl::MeshObject>(
                std::move(boxMesh),
                mBoxShader
        );

        mBoundsCL = make_unique<pbf::Bounds>();
        mBoundsCL->halfDimensions = {1.0f, 1.0f, 1.0f, 0.0f};
        mBoundsCL->dimensions = {2.0f, 2.0f, 2.0f, 0.0f};

        /// Create lights
        mAmbLight = std::make_shared<clgl::AmbientLight>(glm::vec3(0.3f, 0.3f, 1.0f), 0.2f);
        mDirLight = std::make_shared<clgl::DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), 0.1f);
        mPointLight = std::make_shared<clgl::PointLight>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
        mPointLight->setPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        mPointLight->setAttenuation(clgl::Attenuation(0.1f, 0.1f));
    }

    void ParticleSimulationScene::addGUI(nanogui::Screen *screen) {
        auto size = screen->size();

        mCamera->setScreenDimensions(glm::uvec2(size[0], size[1]));
        mCamera->setClipPlanes(0.01f, 100.f);

        using namespace nanogui;
        Window *win = new Window(screen, "Scene Controls");
        win->setPosition(Eigen::Vector2i(15, 125));
        win->setLayout(new GroupLayout());


        /// Fluid scenes
        new Label(win, "Fluid Setups");
        Button *b = new Button(win, "Dam break");
        b->setCallback([&]() {
            this->loadFluidSetup(RESPATH("fluidSetups/dam-break.txt"));
        });
        b = new Button(win, "Cube drop");
        b->setCallback([&]() {
            this->loadFluidSetup(RESPATH("fluidSetups/cube-drop.txt"));
        });


        /// Ambient light parameters

        new Label(win, "Ambient Intensity", "sans", 10);
        Slider *ambIntensity = new Slider(win);
        ambIntensity->setCallback([&](float value){
            mAmbLight->setIntensity(value);
        });
        ambIntensity->setValue(mAmbLight->getIntensity());


        /// Directional light parameters

        new Label(win, "Directional Intensity", "sans", 10);
        Slider *dirIntensity = new Slider(win);
        dirIntensity->setCallback([&](float value){
            mDirLight->setIntensity(value);
        });
        dirIntensity->setValue(mDirLight->getIntensity());


        /// Spot light parameters

        new Label(win, "Point Parameters", "sans", 10);
        new Label(win, "Intensity");
        Slider *spotSlider = new Slider(win);
        spotSlider->setCallback([&](float value){
            mPointLight->setIntensity(value);
        });
        spotSlider->setValue(mPointLight->getIntensity());

        new Label(win, "Attenuation (linear)");
        spotSlider = new Slider(win);
        spotSlider->setCallback([&](float value){
            clgl::Attenuation att = mPointLight->getAttenuation();
            att.a = value * 10;
            mPointLight->setAttenuation(att);
        });
        spotSlider->setValue(mPointLight->getAttenuation().a / 10);

        new Label(win, "Attenuation (quadratic)");
        spotSlider = new Slider(win);
        spotSlider->setCallback([&](float value){
            clgl::Attenuation att = mPointLight->getAttenuation();
            att.b = value * 10;
            mPointLight->setAttenuation(att);
        });
        spotSlider->setValue(mPointLight->getAttenuation().b / 10);
    }

    void ParticleSimulationScene::loadFluidSetup(const std::string &path) {
        std::ifstream ifs(path.c_str());

        const unsigned int MAX_PARTICLES = 10000;
        std::vector<glm::vec4> positions(MAX_PARTICLES);
        std::vector<glm::vec4> velocities(MAX_PARTICLES);
        std::vector<float> densities(MAX_PARTICLES);

        if (ifs.is_open() && !ifs.eof()) {
            ifs >> mNumParticles;

            while (!ifs.eof()) {
                for (unsigned int id = 0; id < mNumParticles; ++id) {
                    ifs >> positions[id].x;
                    ifs >> positions[id].y;
                    ifs >> positions[id].z;
                }
            }
        }

        ifs.close();

        initializeParticleStates(std::move(positions), std::move(velocities), std::move(densities));
    }

    void ParticleSimulationScene::initializeParticleStates(std::vector<glm::vec4> &&positions,
                                                           std::vector<glm::vec4> &&velocities,
                                                           std::vector<float> &&densities) {
        const unsigned int MAX_PARTICLES = positions.size();

        mPositionsGL->bind();
        mPositionsGL->bufferData(4 * sizeof(float) * MAX_PARTICLES, &positions[0]);
        mPositionsGL->unbind();

        mVelocitiesGL->bind();
        mVelocitiesGL->bufferData(4 * sizeof(float) * MAX_PARTICLES, &velocities[0]);
        mVelocitiesGL->unbind();

        mDensitiesGL->bind();
        mDensitiesGL->bufferData(sizeof(float) * MAX_PARTICLES, &densities[0]);
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

        OCL_CALL(mClipToBoundsKernel->setArg(0, *mPositionsCL));
        OCL_CALL(mClipToBoundsKernel->setArg(1, *mVelocitiesCL));
        OCL_CALL(mClipToBoundsKernel->setArg(2, sizeof(pbf::Bounds), mBoundsCL.get()));

        mCamera->setPosition(glm::vec3(0.0f, 0.0f, 15.0f));
        mDirLight->setLightDirection(glm::vec3(-1.0f));
    }

    void ParticleSimulationScene::reset() {
        const unsigned int MAX_PARTICLES = 1000;
        mNumParticles = 1000;
        mDeltaTime = 0.005f;

        std::vector<glm::vec4> positions(MAX_PARTICLES);

        std::vector<float> polarAngles = util::generate_uniform_floats(1000, -CL_M_PI/2, CL_M_PI/2);
        std::vector<float> azimuthalAngles = util::generate_uniform_floats(1000, 0.0f, 2 * CL_M_PI);

        std::vector<glm::vec4> velocities(MAX_PARTICLES);
        {
            glm::vec4 velocity(0);
            float polar, azimuthal;

            for (int i = 0; i < MAX_PARTICLES; ++i) {
                polar = polarAngles[i];
                azimuthal = azimuthalAngles[i];

                velocity.x = sinf(azimuthal) * cosf(polar);
                velocity.y = cosf(azimuthal);
                velocity.z = sinf(azimuthal) * sinf(polar);

                velocities[i] = velocity;
            }
        }

        std::vector<float> densities(MAX_PARTICLES);

        initializeParticleStates(std::move(positions), std::move(velocities), std::move(densities));
    }

    void ParticleSimulationScene::update() {
        cl::Event event;
        OCL_CALL(mQueue.enqueueAcquireGLObjects(&mMemObjects));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mTimestepKernel, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mClipToBoundsKernel, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));
        OCL_CALL(mQueue.enqueueReleaseGLObjects(&mMemObjects, NULL, &event));
        OCL_CALL(event.wait());
    }

    void ParticleSimulationScene::render() {
        OGL_CALL(glEnable(GL_DEPTH_TEST));
        OGL_CALL(glEnable(GL_CULL_FACE));
        OGL_CALL(glCullFace(GL_BACK));

        const glm::mat4 viewProjection = mCamera->getPerspectiveTransform() * glm::inverse(mCamera->getTransform());

        mParticlesShader->use();
        mParticlesShader->uniform("Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        mParticlesShader->uniform("MVP", viewProjection);
        mParticles->bind();
        OGL_CALL(glPointSize(5.0f));
        OGL_CALL(glDrawArrays(GL_POINTS, 0, (GLsizei) mNumParticles));
        mParticles->unbind();

        // Cull front faces to only render box insides
        OGL_CALL(glCullFace(GL_FRONT));
        mAmbLight->setUniformsInShader(mBoxShader, "ambLight.");
        mDirLight->setUniformsInShader(mBoxShader, "dirLight.");
        mPointLight->setUniformsInShader(mBoxShader, "pointLight.");
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
            glm::vec3 eulerAngles = mCameraRotator->getEulerAngles();
            eulerAngles.x += 0.05f * rel.y;
            eulerAngles.y += 0.05f * rel.x;
            eulerAngles.x = clamp(eulerAngles.x, - CL_M_PI_F / 2, CL_M_PI_F / 2);
            mCameraRotator->setEulerAngles(eulerAngles);

            return true;
        }

        return false;
    }

    bool ParticleSimulationScene::resizeEvent(const glm::ivec2 &p) {
        mCamera->setScreenDimensions(glm::uvec2(static_cast<unsigned int>(p.x),
                                                static_cast<unsigned int>(p.y)));
        return false;
    }
}