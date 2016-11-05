#include <OpenCL/opencl.h>
#include "ParticleSimulationScene.hpp"

#include <glm/ext.hpp>
#include "util/paths.hpp"
#include "util/OCL_CALL.hpp"
#include "util/make_unique.hpp"
#include "util/math_util.hpp"
#include "util/cl_util.hpp"

#include "geometry/Primitives.hpp"

#include <iomanip>

#define FIRST_BUFFER 0
#define SECOND_BUFFER 1

namespace pbf {
    using util::make_unique;
    using namespace bwgl;
    using namespace cl;

    ParticleSimulationScene::ParticleSimulationScene(cl::Context &context, cl::Device &device, cl::CommandQueue &queue)
            : BaseScene(context, device, queue) {
        mParticleRadius = 2.0f;
        mCurrentFluidSetup = RESPATH("fluidSetups/dam-break.txt");

        loadShaders();

        /// Create camera
        mCameraRotator = std::make_shared<clgl::SceneObject>();
        mCameraRotator->translate(glm::vec3(0.0f, 0.10f, 0.0f));
        mCamera = std::make_shared<clgl::Camera>(glm::uvec2(100, 100), 50);
        clgl::SceneObject::attach(mCameraRotator, mCamera);

        const glm::vec3 HALFDIMS(0.8f, 1.0f, 1.0f);

        /// Create geometry
        auto boxMesh = clgl::Primitives::CreateBox(HALFDIMS);
        boxMesh->flipNormals();
        mBoundingBox = std::make_shared<clgl::MeshObject>(
                std::move(boxMesh),
                mBoxShader
        );

        mBoundsCL = make_unique<pbf::Bounds>();
        mBoundsCL->halfDimensions = {HALFDIMS.x, HALFDIMS.y, HALFDIMS.z, 0.0f};
        mBoundsCL->dimensions = {2*HALFDIMS.x, 2*HALFDIMS.y, 2*HALFDIMS.z, 0.0f};

        mGridCL = make_unique<pbf::Grid>();
        mGridCL->halfDimensions = {HALFDIMS.x, HALFDIMS.y, HALFDIMS.z, 0.0f};
        mGridCL->binSize = 0.1f;
        mGridCL->binCount3D = {16, 20, 20, 0};
        mGridCL->binCount = 16 * 20 * 20;

        mFluidCL = pbf::Fluid::GetDefault();

        mSpawnPoint = glm::vec2(0.0f, 0.0f);
        mSpawnPointSphere.mRadius = 0.2f;
        mSpawnPointSphere.mPosition = getWorldSpawnPoint();

        auto sphereMesh = clgl::Primitives::CreateIcosphere(0.2f, 2);
        mSpawnPointSphereObject = std::make_shared<clgl::MeshObject>(
                std::move(sphereMesh),
                mBoxShader,
                true
        );
        mSpawnPointSphereObject->setPosition(getWorldSpawnPoint());

        /// Create lights
        mAmbLight = std::make_shared<clgl::AmbientLight>(glm::vec3(0.3f, 0.3f, 1.0f), 0.2f);
        mDirLight = std::make_shared<clgl::DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), 0.1f);
        mPointLight = std::make_shared<clgl::PointLight>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
        mPointLight->setPosition(glm::vec3(0.0f, HALFDIMS.y, 0.0f));
        mPointLight->setAttenuation(clgl::Attenuation(0.1f, 0.1f));


        /// Setup particle attributes as OpenGL buffers
        mPositionsGL[FIRST_BUFFER] = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        mPositionsGL[SECOND_BUFFER] = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        mVelocitiesGL[FIRST_BUFFER] = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        mVelocitiesGL[SECOND_BUFFER] = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        mDensitiesGL = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        mPredictedPositionsGL[FIRST_BUFFER] = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
        mPredictedPositionsGL[SECOND_BUFFER] = make_unique<VertexBuffer>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);


        /// Create OpenGL vertex array, representing each particle
        mParticles[FIRST_BUFFER] = make_unique<VertexArray>();
        mParticles[FIRST_BUFFER]->bind();
        mParticles[FIRST_BUFFER]->addVertexAttribute(*mPositionsGL[FIRST_BUFFER], 4, GL_FLOAT, GL_FALSE, /*3*sizeof(GLfloat)*/ 0);
        mParticles[FIRST_BUFFER]->addVertexAttribute(*mVelocitiesGL[FIRST_BUFFER], 4, GL_FLOAT, GL_FALSE, /*3*sizeof(GLfloat)*/ 0);
        mParticles[FIRST_BUFFER]->addVertexAttribute(*mDensitiesGL, 1, GL_FLOAT, GL_FALSE, /*sizeof(GLfloat)*/ 0);
        mParticles[FIRST_BUFFER]->unbind();

        mParticles[SECOND_BUFFER] = make_unique<VertexArray>();
        mParticles[SECOND_BUFFER]->bind();
        mParticles[SECOND_BUFFER]->addVertexAttribute(*mPositionsGL[SECOND_BUFFER], 4, GL_FLOAT, GL_FALSE, /*3*sizeof(GLfloat)*/ 0);
        mParticles[SECOND_BUFFER]->addVertexAttribute(*mVelocitiesGL[FIRST_BUFFER], 4, GL_FLOAT, GL_FALSE, /*3*sizeof(GLfloat)*/ 0);
        mParticles[SECOND_BUFFER]->addVertexAttribute(*mDensitiesGL, 1, GL_FLOAT, GL_FALSE, /*sizeof(GLfloat)*/ 0);
        mParticles[SECOND_BUFFER]->unbind();

        loadKernels();
    }

    void ParticleSimulationScene::addGUI(nanogui::Screen *screen) {
        auto size = screen->size();

        mCamera->setScreenDimensions(glm::uvec2(size[0], size[1]));
        mCamera->setClipPlanes(0.01f, 100.f);

        using namespace nanogui;
        Window *win = new Window(screen, "Scene Controls");
        win->setPosition(Eigen::Vector2i(15, 125));
        win->setLayout(new GroupLayout());

        Button *b = new Button(win, "Reload shaders");
        b->setCallback([this]() {
            loadShaders();
            mBoundingBox->setShader(mBoxShader);
        });
        b = new Button(win, "Reload kernels");
        b->setCallback([this]() {
            loadKernels();
        });

        /// Fluid scenes
        new Label(win, "Fluid Setups");
        b = new Button(win, "Dam break");
        b->setCallback([&]() {
            mCurrentFluidSetup = RESPATH("fluidSetups/dam-break.txt");
            this->loadFluidSetup(mCurrentFluidSetup);
        });
        b = new Button(win, "Larger dam break");
        b->setCallback([&]() {
            mCurrentFluidSetup = RESPATH("fluidSetups/large-dam-break.txt");
            this->loadFluidSetup(mCurrentFluidSetup);
        });
        b = new Button(win, "Cube drop");
        b->setCallback([&]() {
            mCurrentFluidSetup = RESPATH("fluidSetups/cube-drop.txt");
            this->loadFluidSetup(mCurrentFluidSetup);
        });

        /// FPS Labels
        mLabelAverageFrameTime = new Label(win, "");
        mLabelFPS = new Label(win, "");
        updateTimeLabelsInGUI(0.0);

        /// Particles size
        new Label(win, "Particle size");
        Slider *particleSize = new Slider(win);
        particleSize->setCallback([&](float value) {
            mParticleRadius = 20 * value;
        });
        particleSize->setValue(mParticleRadius / 20);

        FormHelper *gui = new FormHelper(screen);
        gui->addWindow(Eigen::Vector2i(screen->width() - 200, 15), "Fluid parameters");

        gui->addButton("Load", [&, gui] {
            std::string filename = file_dialog({ {"txt", "Text file"}, {"txt", "Text file"} }, false);
            pbf::Fluid::ReadFromFile(filename, *mFluidCL);
            gui->refresh();
        });
        gui->addButton("Save", [&, gui] {
            std::string filename = file_dialog({ {"txt", "Text file"}, {"txt", "Text file"} }, true);
            pbf::Fluid::WriteToFile(filename, *mFluidCL);
            gui->refresh();
        });

        gui->addVariable("Sub-steps", mFluidCL->numSubSteps);
        gui->addVariable("kernelRadius", mFluidCL->kernelRadius, false);
        gui->addVariable("restDensity", mFluidCL->restDensity);
        gui->addVariable("deltaTime", mFluidCL->deltaTime);
        gui->addVariable("epsilon", mFluidCL->epsilon);
        gui->addVariable("k", mFluidCL->k);
        gui->addVariable("delta_q", mFluidCL->delta_q);
        gui->addVariable("n", mFluidCL->n);
        gui->addVariable("c", mFluidCL->c);
        gui->addVariable("k_vc", mFluidCL->k_vc);
        gui->addVariable("kBoundsDensity", mFluidCL->kBoundsDensity);
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
        mPositionsGL[FIRST_BUFFER]->bind();
        mPositionsGL[FIRST_BUFFER]->bufferData(4 * sizeof(float) * NUM_MAX_PARTICLES, &positions[0]);
        mPositionsGL[FIRST_BUFFER]->unbind();

        mVelocitiesGL[FIRST_BUFFER]->bind();
        mVelocitiesGL[FIRST_BUFFER]->bufferData(4 * sizeof(float) * NUM_MAX_PARTICLES, &velocities[0]);
        mVelocitiesGL[FIRST_BUFFER]->unbind();

        mPositionsGL[SECOND_BUFFER]->bind();
        mPositionsGL[SECOND_BUFFER]->bufferData(4 * sizeof(float) * NUM_MAX_PARTICLES, &positions[0]);
        mPositionsGL[SECOND_BUFFER]->unbind();

        mVelocitiesGL[SECOND_BUFFER]->bind();
        mVelocitiesGL[SECOND_BUFFER]->bufferData(4 * sizeof(float) * NUM_MAX_PARTICLES, &velocities[0]);
        mVelocitiesGL[SECOND_BUFFER]->unbind();

        mPredictedPositionsGL[FIRST_BUFFER]->bind();
        mPredictedPositionsGL[FIRST_BUFFER]->bufferData(4 * sizeof(float) * NUM_MAX_PARTICLES, &positions[0]);
        mPredictedPositionsGL[FIRST_BUFFER]->unbind();

        mPredictedPositionsGL[SECOND_BUFFER]->bind();
        mPredictedPositionsGL[SECOND_BUFFER]->bufferData(4 * sizeof(float) * NUM_MAX_PARTICLES, &positions[0]);
        mPredictedPositionsGL[SECOND_BUFFER]->unbind();

        mDensitiesGL->bind();
        mDensitiesGL->bufferData(sizeof(float) * NUM_MAX_PARTICLES, &densities[0]);
        mDensitiesGL->unbind();

        /// Create OpenCL references to OpenGL buffers
        OCL_ERROR;
        OCL_CHECK(mPositionsCL[FIRST_BUFFER] = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mPositionsGL[FIRST_BUFFER]->ID(), CL_ERROR));
        mMemObjects.push_back(*mPositionsCL[FIRST_BUFFER]);
        OCL_CHECK(mPositionsCL[SECOND_BUFFER] = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mPositionsGL[SECOND_BUFFER]->ID(), CL_ERROR));
        mMemObjects.push_back(*mPositionsCL[SECOND_BUFFER]);

        OCL_CHECK(mVelocitiesCL[FIRST_BUFFER] = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mVelocitiesGL[FIRST_BUFFER]->ID(), CL_ERROR));
        mMemObjects.push_back(*mVelocitiesCL[FIRST_BUFFER]);
        OCL_CHECK(mVelocitiesCL[SECOND_BUFFER] = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mVelocitiesGL[SECOND_BUFFER]->ID(), CL_ERROR));
        mMemObjects.push_back(*mVelocitiesCL[SECOND_BUFFER]);

        OCL_CHECK(mPredictedPositionsCL[FIRST_BUFFER] = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mPredictedPositionsGL[FIRST_BUFFER]->ID(), CL_ERROR));
        mMemObjects.push_back(*mPredictedPositionsCL[FIRST_BUFFER]);
        OCL_CHECK(mPredictedPositionsCL[SECOND_BUFFER] = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mPredictedPositionsGL[SECOND_BUFFER]->ID(), CL_ERROR));
        mMemObjects.push_back(*mPredictedPositionsCL[SECOND_BUFFER]);

        OCL_CHECK(mDensitiesCL = make_unique<BufferGL>(mContext, CL_MEM_READ_WRITE, mDensitiesGL->ID(), CL_ERROR));
        mMemObjects.push_back(*mDensitiesCL);

        /// Setup CL-only buffers (for the grid)
        OCL_CHECK(mBinCountCL = make_unique<cl::Buffer>(mContext, CL_MEM_READ_WRITE, sizeof(cl_uint) * mGridCL->binCount, (void*)0, CL_ERROR));
        OCL_CHECK(mBinStartIDCL = make_unique<cl::Buffer>(mContext, CL_MEM_READ_WRITE, sizeof(cl_uint) * mGridCL->binCount, (void*)0, CL_ERROR));
        OCL_CHECK(mParticleInBinPosCL = make_unique<cl::Buffer>(mContext, CL_MEM_READ_WRITE, sizeof(cl_uint) * NUM_MAX_PARTICLES, (void*)0, CL_ERROR));
        OCL_CHECK(mParticleBinIDCL[FIRST_BUFFER] = make_unique<cl::Buffer>(mContext, CL_MEM_READ_WRITE, sizeof(cl_uint) * NUM_MAX_PARTICLES, (void*)0, CL_ERROR));
        OCL_CHECK(mParticleBinIDCL[SECOND_BUFFER] = make_unique<cl::Buffer>(mContext, CL_MEM_READ_WRITE, sizeof(cl_uint) * NUM_MAX_PARTICLES, (void*)0, CL_ERROR));
        OCL_CHECK(mParticleLambdasCL = make_unique<cl::Buffer>(mContext, CL_MEM_READ_WRITE, sizeof(cl_float) * NUM_MAX_PARTICLES, (void*)0, CL_ERROR));
        OCL_CHECK(mParticleCurlsCL = make_unique<cl::Buffer>(mContext, CL_MEM_READ_WRITE, sizeof(cl_float3) * NUM_MAX_PARTICLES, (void*)0, CL_ERROR));

        OCL_CALL(mQueue.enqueueFillBuffer<cl_uint>(*mBinCountCL, 0, 0, sizeof(cl_uint) * mGridCL->binCount));
        OCL_CALL(mQueue.enqueueFillBuffer<cl_uint>(*mBinStartIDCL, 0, 0, sizeof(cl_uint) * mGridCL->binCount));
        OCL_CALL(mQueue.enqueueFillBuffer<cl_uint>(*mParticleInBinPosCL, 0, 0, sizeof(cl_uint) * NUM_MAX_PARTICLES));
        OCL_CALL(mQueue.enqueueFillBuffer<cl_uint>(*mParticleBinIDCL[FIRST_BUFFER], 0, 0, sizeof(cl_uint) * NUM_MAX_PARTICLES));
        OCL_CALL(mQueue.enqueueFillBuffer<cl_uint>(*mParticleBinIDCL[SECOND_BUFFER], 0, 0, sizeof(cl_uint) * NUM_MAX_PARTICLES));

        /// Set these arguments of the kernel since they don't flip their buffers
        OCL_CALL(mSortInsertParticles->setArg(2, *mParticleInBinPosCL));
        OCL_CALL(mSortInsertParticles->setArg(3, *mBinCountCL));
        OCL_CALL(mSortComputeBinStartID->setArg(0, *mBinCountCL));
        OCL_CALL(mSortComputeBinStartID->setArg(1, *mBinStartIDCL));
        OCL_CALL(mSortReindexParticles->setArg(1, *mParticleInBinPosCL));
        OCL_CALL(mSortReindexParticles->setArg(2, *mBinStartIDCL));

        mCamera->setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
        mDirLight->setLightDirection(glm::vec3(-1.0f));
    }

    void ParticleSimulationScene::reset() {
        mCurrentBufferID = 0;
        loadFluidSetup(mCurrentFluidSetup);

        while (!mSimulationTimes.empty()) {
            mSimulationTimes.pop_back();
        }
        updateTimeLabelsInGUI(0.0);
    }

    void ParticleSimulationScene::update() {
        /// Move spawn point
        if (isKeyDown(GLFW_KEY_RIGHT)) mSpawnPoint.x += 0.018f;
        if (isKeyDown(GLFW_KEY_LEFT)) mSpawnPoint.x -= 0.018f;
        if (isKeyDown(GLFW_KEY_UP)) mSpawnPoint.y += 0.018f;
        if (isKeyDown(GLFW_KEY_DOWN)) mSpawnPoint.y -= 0.018f;
        mSpawnPointSphereObject->setPosition(glm::vec3(getWorldSpawnPoint().x, -getWorldSpawnPoint().y, getWorldSpawnPoint().z));
        if (isKeyDown(GLFW_KEY_SPACE)) spawnParticles();

        /// Rotate camera with keys
        glm::vec3 eulerAngles = mCameraRotator->getEulerAngles();
        if (isKeyDown(GLFW_KEY_A)) eulerAngles.y += 0.04f;
        if (isKeyDown(GLFW_KEY_D)) eulerAngles.y -= 0.04f;
        if (isKeyDown(GLFW_KEY_S)) eulerAngles.x -= 0.04f;
        if (isKeyDown(GLFW_KEY_W)) eulerAngles.x += 0.04f;
        eulerAngles.x = clamp(eulerAngles.x, - CL_M_PI_F / 2, CL_M_PI_F / 2);
        mCameraRotator->setEulerAngles(eulerAngles);

        double timeBegin = glfwGetTime();
        if (mFramesSinceLastUpdate == 0) {
            mTimeOfLastUpdate = timeBegin;
        }

        ++mFramesSinceLastUpdate;

        unsigned int previousBufferID = mCurrentBufferID;
        //unsigned int previousBufferID = 0;
        mCurrentBufferID = 1 - mCurrentBufferID;

        cl::Event event;
        OCL_CALL(mQueue.enqueueAcquireGLObjects(&mMemObjects));

        ///////////////////////////////////////////////////
        /// Apply external forces and predict positions ///
        ///////////////////////////////////////////////////

        OCL_CALL(mTimestepKernel->setArg(0, *mPositionsCL[previousBufferID]));
        OCL_CALL(mTimestepKernel->setArg(1, *mPredictedPositionsCL[previousBufferID]));
        OCL_CALL(mTimestepKernel->setArg(2, *mVelocitiesCL[FIRST_BUFFER]));
        OCL_CALL(mTimestepKernel->setArg(3, mFluidCL->deltaTime));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mTimestepKernel, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));

        OCL_CALL(mClipToBoundsKernel->setArg(0, *mPredictedPositionsCL[previousBufferID]));
        OCL_CALL(mClipToBoundsKernel->setArg(1, sizeof(pbf::Bounds), mBoundsCL.get()));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mClipToBoundsKernel, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));

        /////////////////////
        /// Counting sort ///
        /////////////////////

        /// Reset bin counts to zero
        OCL_CALL(mQueue.enqueueFillBuffer<cl_uint>(*mBinCountCL, 0, 0, sizeof(cl_uint) * mGridCL->binCount));

        // Insert particles based on their predicted positions
        OCL_CALL(mSortInsertParticles->setArg(0, *mPredictedPositionsCL[previousBufferID]));
        OCL_CALL(mSortInsertParticles->setArg(1, *mParticleBinIDCL[previousBufferID]));
        OCL_CALL(mSortInsertParticles->setArg(2, *mParticleInBinPosCL));
        OCL_CALL(mSortInsertParticles->setArg(3, *mBinCountCL));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mSortInsertParticles, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));

        OCL_CALL(mSortComputeBinStartID->setArg(0, *mBinCountCL));
        OCL_CALL(mSortComputeBinStartID->setArg(1, *mBinStartIDCL));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mSortComputeBinStartID, cl::NullRange,
                                             cl::NDRange(mGridCL->binCount, 1), cl::NullRange));

        OCL_CALL(mSortReindexParticles->setArg(0, *mParticleInBinPosCL));
        OCL_CALL(mSortReindexParticles->setArg(1, *mBinStartIDCL));
        OCL_CALL(mSortReindexParticles->setArg(2, *mPositionsCL[previousBufferID]));
        OCL_CALL(mSortReindexParticles->setArg(3, *mPredictedPositionsCL[previousBufferID]));
        OCL_CALL(mSortReindexParticles->setArg(4, *mVelocitiesCL[FIRST_BUFFER]));
        OCL_CALL(mSortReindexParticles->setArg(5, *mParticleBinIDCL[previousBufferID]));
        OCL_CALL(mSortReindexParticles->setArg(6, *mPositionsCL[mCurrentBufferID]));
        OCL_CALL(mSortReindexParticles->setArg(7, *mPredictedPositionsCL[mCurrentBufferID]));
        OCL_CALL(mSortReindexParticles->setArg(8, *mVelocitiesCL[SECOND_BUFFER]));
        OCL_CALL(mSortReindexParticles->setArg(9, *mParticleBinIDCL[mCurrentBufferID]));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mSortReindexParticles, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));

        //////////////////////////////////
        /// Apply position corrections ///
        //////////////////////////////////

        /// Reset densities to zero
        OCL_CALL(mQueue.enqueueFillBuffer<cl_uint>(*mDensitiesCL, 0, 0, sizeof(cl_float) * mNumParticles));

        for (unsigned int i = 0; i < mFluidCL->numSubSteps; ++i) {
            ////////////////////
            /// Calculate λi ///
            ////////////////////

            /// Calculate densities
            OCL_CALL(mCalcDensities->setArg(0, sizeof(pbf::Fluid), mFluidCL.get()));
            OCL_CALL(mCalcDensities->setArg(1, sizeof(pbf::Bounds), mBoundsCL.get()));
            OCL_CALL(mCalcDensities->setArg(2, *mPredictedPositionsCL[mCurrentBufferID]));
            OCL_CALL(mCalcDensities->setArg(3, *mParticleBinIDCL[mCurrentBufferID]));
            OCL_CALL(mCalcDensities->setArg(4, *mBinStartIDCL));
            OCL_CALL(mCalcDensities->setArg(5, *mBinCountCL));
            OCL_CALL(mCalcDensities->setArg(6, *mDensitiesCL));
            OCL_CALL(mQueue.enqueueNDRangeKernel(*mCalcDensities, cl::NullRange,
                                                 cl::NDRange(mNumParticles, 1), cl::NullRange));

            /// Calculate λi
            OCL_CALL(mCalcLambdas->setArg(0, sizeof(pbf::Fluid), mFluidCL.get()));
            OCL_CALL(mCalcLambdas->setArg(1, *mPredictedPositionsCL[mCurrentBufferID]));
            OCL_CALL(mCalcLambdas->setArg(2, *mParticleBinIDCL[mCurrentBufferID]));
            OCL_CALL(mCalcLambdas->setArg(3, *mBinStartIDCL));
            OCL_CALL(mCalcLambdas->setArg(4, *mBinCountCL));
            OCL_CALL(mCalcLambdas->setArg(5, *mDensitiesCL));
            OCL_CALL(mCalcLambdas->setArg(6, *mParticleLambdasCL));
            OCL_CALL(mQueue.enqueueNDRangeKernel(*mCalcLambdas, cl::NullRange,
                                                 cl::NDRange(mNumParticles, 1), cl::NullRange));


            ////////////////////////////////////////////////
            /// calculate ∆pi                            ///
            /// perform collision detection and response ///
            ////////////////////////////////////////////////

            /// calculate ∆pi and update x*i
            OCL_CALL(mCalcDeltaPositionAndDoUpdate->setArg(0, sizeof(pbf::Fluid), mFluidCL.get()));
            OCL_CALL(mCalcDeltaPositionAndDoUpdate->setArg(1, sizeof(pbf::Bounds), mFluidCL.get()));
            OCL_CALL(mCalcDeltaPositionAndDoUpdate->setArg(2, *mPredictedPositionsCL[mCurrentBufferID]));
            OCL_CALL(mCalcDeltaPositionAndDoUpdate->setArg(3, *mParticleBinIDCL[mCurrentBufferID]));
            OCL_CALL(mCalcDeltaPositionAndDoUpdate->setArg(4, *mBinStartIDCL));
            OCL_CALL(mCalcDeltaPositionAndDoUpdate->setArg(5, *mBinCountCL));
            OCL_CALL(mCalcDeltaPositionAndDoUpdate->setArg(6, *mDensitiesCL));
            OCL_CALL(mCalcDeltaPositionAndDoUpdate->setArg(7, *mParticleLambdasCL));
            OCL_CALL(mQueue.enqueueNDRangeKernel(*mCalcDeltaPositionAndDoUpdate, cl::NullRange,
                                                 cl::NDRange(mNumParticles, 1), cl::NullRange));

            OCL_CALL(mClipToBoundsKernel->setArg(0, *mPredictedPositionsCL[mCurrentBufferID]));
            OCL_CALL(mClipToBoundsKernel->setArg(1, sizeof(pbf::Bounds), mBoundsCL.get()));
            OCL_CALL(mQueue.enqueueNDRangeKernel(*mClipToBoundsKernel, cl::NullRange,
                                                 cl::NDRange(mNumParticles, 1), cl::NullRange));

            ////////////////////////////////////////
            /// update position x∗i ⇐ x∗i + ∆pi ///
            ////////////////////////////////////////
        }

        //////////////////////////////////////////////////////
        /// update velocity vi ⇐ (1/∆t)(x∗i − xi)         ///
        /// apply vorticity confinement and XSPH viscosity ///
        /// update position xi ⇐ x∗i                      ///
        //////////////////////////////////////////////////////

        OCL_CALL(mRecalcVelocities->setArg(0, *mPositionsCL[mCurrentBufferID]));
        OCL_CALL(mRecalcVelocities->setArg(1, *mPredictedPositionsCL[mCurrentBufferID]));
        OCL_CALL(mRecalcVelocities->setArg(2, *mVelocitiesCL[SECOND_BUFFER]));
        OCL_CALL(mRecalcVelocities->setArg(3, 1.0f / mFluidCL->deltaTime));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mRecalcVelocities, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));

        OCL_CALL(mCalcCurls->setArg(0, sizeof(pbf::Fluid), mFluidCL.get()));
        OCL_CALL(mCalcCurls->setArg(1, *mParticleBinIDCL[mCurrentBufferID]));
        OCL_CALL(mCalcCurls->setArg(2, *mBinStartIDCL));
        OCL_CALL(mCalcCurls->setArg(3, *mBinCountCL));
        OCL_CALL(mCalcCurls->setArg(4, *mPredictedPositionsCL[mCurrentBufferID]));
        OCL_CALL(mCalcCurls->setArg(5, *mVelocitiesCL[SECOND_BUFFER]));
        OCL_CALL(mCalcCurls->setArg(6, *mParticleCurlsCL));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mCalcCurls, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));

        OCL_CALL(mApplyVortAndViscXSPH->setArg(0, sizeof(pbf::Fluid), mFluidCL.get()));
        OCL_CALL(mApplyVortAndViscXSPH->setArg(1, *mParticleBinIDCL[mCurrentBufferID]));
        OCL_CALL(mApplyVortAndViscXSPH->setArg(2, *mBinStartIDCL));
        OCL_CALL(mApplyVortAndViscXSPH->setArg(3, *mBinCountCL));
        OCL_CALL(mApplyVortAndViscXSPH->setArg(4, *mPredictedPositionsCL[mCurrentBufferID]));
        OCL_CALL(mApplyVortAndViscXSPH->setArg(5, *mDensitiesCL));
        OCL_CALL(mApplyVortAndViscXSPH->setArg(6, *mParticleCurlsCL));
        OCL_CALL(mApplyVortAndViscXSPH->setArg(7, *mVelocitiesCL[SECOND_BUFFER]));
        OCL_CALL(mApplyVortAndViscXSPH->setArg(8, *mVelocitiesCL[FIRST_BUFFER]));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mApplyVortAndViscXSPH, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));

        OCL_CALL(mSetPositionsFromPredictions->setArg(0, *mPredictedPositionsCL[mCurrentBufferID]));
        OCL_CALL(mSetPositionsFromPredictions->setArg(1, *mPositionsCL[mCurrentBufferID]));
        OCL_CALL(mQueue.enqueueNDRangeKernel(*mSetPositionsFromPredictions, cl::NullRange,
                                             cl::NDRange(mNumParticles, 1), cl::NullRange));

        OCL_CALL(mQueue.enqueueReleaseGLObjects(&mMemObjects, NULL, &event));
        OCL_CALL(event.wait());

        double timeEnd = glfwGetTime();
        while (mSimulationTimes.size() > NUM_AVG_SIM_TIMES) {
            mSimulationTimes.pop_back();
        }
        mSimulationTimes.push_front(timeEnd - timeBegin);

        // update GUI twice each second
        if (timeEnd - mTimeOfLastUpdate > 2.0f) {
            updateTimeLabelsInGUI(timeEnd - mTimeOfLastUpdate);
            mFramesSinceLastUpdate = 0;
        }
    }

    void ParticleSimulationScene::render() {
        OGL_CALL(glEnable(GL_DEPTH_TEST));
        OGL_CALL(glEnable(GL_CULL_FACE));
        OGL_CALL(glCullFace(GL_BACK));
        //OGL_CALL(glDisable(GL_PROGRAM_POINT_SIZE));

        const glm::mat4 V = glm::inverse(mCamera->getTransform());
        const glm::mat4 VP = mCamera->getPerspectiveTransform() * V;

        mParticlesShader->use();
        mParticlesShader->uniform("MV", V);
        mParticlesShader->uniform("MVP", VP);
        mParticlesShader->uniform("pointRadius", mParticleRadius);
        mParticlesShader->uniform("pointScale",
                                  mCamera->getScreenDimensions().y / tanf(mCamera->getFieldOfViewY() * CL_M_PI_F / 360));

        mPointLight->setUniformsInShader(mParticlesShader, "pointLight");
        mAmbLight->setUniformsInShader(mParticlesShader, "ambLight");

        mParticles[mCurrentBufferID]->bind();
        OGL_CALL(glPointSize(mParticleRadius));
        OGL_CALL(glDrawArrays(GL_POINTS, 0, (GLsizei) mNumParticles));
        mParticles[mCurrentBufferID]->unbind();

        // Cull front faces to only render box insides
        OGL_CALL(glCullFace(GL_FRONT));
        mAmbLight->setUniformsInShader(mBoxShader, "ambLight.");
        mDirLight->setUniformsInShader(mBoxShader, "dirLight.");
        mPointLight->setUniformsInShader(mBoxShader, "pointLight.");

        mBoundingBox->render(VP);
        mSpawnPointSphereObject->render(VP);
    }

    void ParticleSimulationScene::spawnParticles() {
        const float delta = 0.05f * 6400 / mFluidCL->restDensity;
        const float radius = 4 * delta;

        const float initialVelocity = delta / mFluidCL->deltaTime;
        const glm::vec3 spawnPoint = getWorldSpawnPoint();

        std::vector<glm::vec4> newPositions;

        glm::vec3 position;
        for (int z = -4; z <= 4; ++z) {
            for (int y = -4; y <= 4; ++y) {
                position.x = spawnPoint.x + delta;
                position.y = spawnPoint.y + delta * y;
                position.z = spawnPoint.z + delta * z;

                if (powf(position.y - spawnPoint.y, 2) + powf(position.z - spawnPoint.z, 2) < powf(radius, 2)) {
                    newPositions.push_back(glm::vec4(position, 1.0f));
                }
            }
        }

        const uint nNewParticles = std::min(static_cast<uint>(newPositions.size()) + mNumParticles, NUM_MAX_PARTICLES) - mNumParticles;

        std::vector<glm::vec4> newVelocities(nNewParticles);
        std::fill(newVelocities.begin(), newVelocities.end(), glm::vec4(initialVelocity, 0.0f, 0.0f, 0.0f));

        /// Push to OpenGL buffer
        mPositionsGL[mCurrentBufferID]->bind();
        mPositionsGL[mCurrentBufferID]->bufferSubData(mNumParticles * sizeof(glm::vec4), nNewParticles * sizeof(glm::vec4), newPositions.data());
        mPositionsGL[mCurrentBufferID]->unbind();
        mVelocitiesGL[FIRST_BUFFER]->bind();
        mVelocitiesGL[FIRST_BUFFER]->bufferSubData(mNumParticles * sizeof(glm::vec4), nNewParticles * sizeof(glm::vec4), newVelocities.data());
        mVelocitiesGL[FIRST_BUFFER]->unbind();

        mNumParticles += nNewParticles;
    }

    glm::vec3 ParticleSimulationScene::getWorldSpawnPoint() {
        return glm::vec3(-mBoundsCL->halfDimensions.s[0],
                         mSpawnPoint.y * mBoundsCL->halfDimensions.s[1],
                         mSpawnPoint.x * mBoundsCL->halfDimensions.s[2]);
    }

    bool ParticleSimulationScene::clickedOnSphere(const clgl::Sphere &sphere, const glm::ivec2 &cursorPosition) {
        /// Step 1: 3D normalized device coordinates
        const float x = (2.0f * cursorPosition.x) / mCamera->getScreenDimensions().x - 1.0f;
        const float y = 1.0f - (2.0f * cursorPosition.y) / mCamera->getScreenDimensions().y;
        const float z = 1.0f;

        /// Step 2: 4D homogeneous clip coordinates
        const glm::vec4 rayClip(x, y, -1.0f, 1.0f);

        /// Step 3: 4D camera coordinates
        glm::vec4 rayCamera = glm::inverse(mCamera->getPerspectiveTransform()) * rayClip;
        rayCamera.z = -1.0f;
        rayCamera.w = 0.0f;

        /// Step 4: 4D world coordinates
        glm::vec3 rayWorld(mCamera->getTransform() * rayCamera);
        rayWorld = glm::normalize(rayWorld);

        const glm::vec3 rayOrigin = glm::vec3(mCamera->getParent()->getTransform() * glm::vec4(mCamera->getPosition(), 1.0f));
        glm::vec3 m = rayOrigin - sphere.mPosition;

        float b = glm::dot(m, rayWorld);
        float c = glm::dot(m, m) - sphere.mRadius * sphere.mRadius;

        // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
        if (c > 0.0f && b > 0.0f) return false;
        float discr = b*b - c;

        // A negative discriminant corresponds to ray missing sphere
        return discr >= 0.0f;

    }

    //////////////////////
    /// INPUT HANDLING ///
    //////////////////////

    bool ParticleSimulationScene::keyboardEvent(int key, int scancode, int action, int modifiers) {
        return false;
    }

    bool ParticleSimulationScene::mouseButtonEvent(const glm::ivec2 &p, int button, bool down, int modifiers) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
//            if (clickedOnSphere(mSpawnPointSphere, p)) {
//                std::cout << "Clicked on sphere!" << std::endl;
//                return true;
//            } else {
//                std::cout << "Missed the sphere!" << std::endl;
//            }

            mIsRotatingCamera = down;
            return true;
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

    void ParticleSimulationScene::updateTimeLabelsInGUI(double timeSinceLastUpdate) {
        double cumSimulationTimes = 0.0;
        for (double simTime : mSimulationTimes) {
            cumSimulationTimes += simTime;
        }
        double avgSimulationTime = cumSimulationTimes / mSimulationTimes.size();
        std::stringstream ss;
        ss << "MS/frame: " << std::setprecision(3) << 1000 * avgSimulationTime;
        mLabelAverageFrameTime->setCaption(ss.str());

        ss.str("");

        double FPS = mFramesSinceLastUpdate / timeSinceLastUpdate;
        ss << "Average FPS: " << std::setprecision(3) << FPS;
        mLabelFPS->setCaption(ss.str());
    }

    void ParticleSimulationScene::loadShaders() {
        /// Create shaders
        mParticlesShader = std::make_shared<clgl::BaseShader>(
                std::unordered_map<GLuint, std::string>{{GL_VERTEX_SHADER,   SHADERPATH("particles.vert")},
                                                        {GL_FRAGMENT_SHADER, SHADERPATH("particles.frag")}});
        mParticlesShader->compile();

        mBoxShader = std::make_shared<clgl::BaseShader>(
                std::unordered_map<GLuint, std::string>{{GL_VERTEX_SHADER,   SHADERPATH("box.vert")},
                                                        {GL_FRAGMENT_SHADER, SHADERPATH("box.frag")}});
        mBoxShader->compile();
    }

    void ParticleSimulationScene::loadKernels() {
        OCL_ERROR;

        /// Setup counting sort kernels
        mCountingSortProgram = util::LoadCLProgram("counting_sort.cl", mContext, mDevice, GetDefinesCL(*mGridCL));
        OCL_CHECK(mSortInsertParticles = make_unique<Kernel>(*mCountingSortProgram, "insert_particles", CL_ERROR));
        OCL_CHECK(mSortComputeBinStartID = make_unique<Kernel>(*mCountingSortProgram, "compute_bin_start_ID", CL_ERROR));
        OCL_CHECK(mSortReindexParticles = make_unique<Kernel>(*mCountingSortProgram, "reindex_particles", CL_ERROR));

        /// Setup position adjustment kernels
        mPositionAdjustmentProgram = util::LoadCLProgram("fluid_sim.cl", mContext, mDevice, GetDefinesCL(*mGridCL));
        OCL_CHECK(mCalcDensities = make_unique<Kernel>(*mPositionAdjustmentProgram, "calc_densities", CL_ERROR));
        OCL_CHECK(mCalcLambdas = make_unique<Kernel>(*mPositionAdjustmentProgram, "calc_lambdas", CL_ERROR));
        OCL_CHECK(mCalcDeltaPositionAndDoUpdate = make_unique<Kernel>(*mPositionAdjustmentProgram, "calc_delta_pi_and_update", CL_ERROR));
        OCL_CHECK(mRecalcVelocities = make_unique<Kernel>(*mPositionAdjustmentProgram, "recalc_velocities", CL_ERROR));
        OCL_CHECK(mCalcCurls = make_unique<Kernel>(*mPositionAdjustmentProgram, "calc_curls", CL_ERROR));
        OCL_CHECK(mApplyVortAndViscXSPH = make_unique<Kernel>(*mPositionAdjustmentProgram, "apply_vort_and_viscXSPH", CL_ERROR));
        OCL_CHECK(mSetPositionsFromPredictions = make_unique<Kernel>(*mPositionAdjustmentProgram, "set_positions_from_predictions", CL_ERROR));

        /// Setup timestep kernel
        mTimestepProgram = util::LoadCLProgram("timestep.cl", mContext, mDevice);
        OCL_CHECK(mTimestepKernel = make_unique<Kernel>(*mTimestepProgram, "timestep", CL_ERROR));

        /// Setup "clip to bounds"-kernel
        mClipToBoundsProgram = util::LoadCLProgram("clip_to_bounds.cl", mContext, mDevice);
        OCL_CHECK(mClipToBoundsKernel = make_unique<Kernel>(*mClipToBoundsProgram, "clip_to_bounds", CL_ERROR));

        OCL_CALL(mClipToBoundsKernel->setArg(1, sizeof(pbf::Bounds), mBoundsCL.get()));
    }

    const uint ParticleSimulationScene::NUM_AVG_SIM_TIMES = 10;

    const uint ParticleSimulationScene::NUM_MAX_PARTICLES = 10000;
}