#pragma once

#include "BaseScene.hpp"

#include <bwgl/bwgl.hpp>

#include "rendering/BaseShader.hpp"
#include "rendering/Camera.hpp"
#include "rendering/MeshObject.hpp"
#include "rendering/light/DirectionalLight.hpp"
#include "rendering/light/AmbientLight.hpp"
#include "rendering/light/PointLight.hpp"

#include "simulation/Bounds.hpp"
#include "simulation/Grid.hpp"
#include "simulation/Fluid.hpp"

#include <deque>

namespace pbf {
    /// @brief //todo add brief description to FluidScene
    /// @author Benjamin Wiberg
    class ParticleSimulationScene : public clgl::BaseScene {
    public:
        ParticleSimulationScene(cl::Context &context, cl::Device &device, cl::CommandQueue &queue);

        virtual void addGUI(nanogui::Screen *screen) override;

        virtual void reset() override;

        virtual void update() override;

        virtual void render() override;

        virtual bool mouseButtonEvent(const glm::ivec2 &p, int button, bool down, int modifiers) override;

        virtual bool mouseMotionEvent(const glm::ivec2 &p, const glm::ivec2 &rel, int button, int modifiers) override;

        virtual bool resizeEvent(const glm::ivec2 &p) override;

    private:
        void loadFluidSetup(const std::string &path);

        void loadShaders();

        void loadKernels();

        void initializeParticleStates(std::vector<glm::vec4> && positions,
                                      std::vector<glm::vec4> && velocities,
                                      std::vector<float> && densities);

        std::shared_ptr<clgl::Camera> mCamera;

        std::shared_ptr<clgl::SceneObject> mCameraRotator;

        std::shared_ptr<clgl::MeshObject> mBoundingBox;

        std::shared_ptr<clgl::DirectionalLight> mDirLight;
        std::shared_ptr<clgl::AmbientLight> mAmbLight;
        std::shared_ptr<clgl::PointLight> mPointLight;

        bool mIsRotatingCamera;

        unsigned int mNumParticles;

        std::string mCurrentFluidSetup;

        float mParticleRadius;

        unsigned int mNumSolverIterations;

        std::shared_ptr<clgl::BaseShader> mParticlesShader;
        std::shared_ptr<clgl::BaseShader> mBoxShader;

        /// Keeps track of which of the two buffers is in use this frame
        unsigned int mCurrentBufferID;

        /// OpenGL particle buffers
        /// Double state buffers (pos and vel) are needed for the counting sort algorithm
        std::unique_ptr<bwgl::VertexBuffer> mPositionsGL[2];
        std::unique_ptr<bwgl::VertexBuffer> mPredictedPositionsGL[2]; // unused in GL
        std::unique_ptr<bwgl::VertexBuffer> mVelocitiesGL[2];
        std::unique_ptr<bwgl::VertexBuffer> mDensitiesGL;
        std::unique_ptr<bwgl::VertexArray> mParticles[2];

        /// OpenCL particle buffers
        std::unique_ptr<cl::BufferGL> mPositionsCL[2];
        std::unique_ptr<cl::BufferGL> mPredictedPositionsCL[2];
        std::unique_ptr<cl::BufferGL> mVelocitiesCL[2];
        std::unique_ptr<cl::BufferGL> mDensitiesCL;
        std::unique_ptr<cl::Buffer> mParticleBinIDCL[2];
        std::unique_ptr<cl::Buffer> mParticleLambdasCL;

        /// OpenCL stuff
        std::unique_ptr<pbf::Bounds> mBoundsCL;
        std::unique_ptr<pbf::Grid> mGridCL;
        std::unique_ptr<pbf::Fluid> mFluidCL;

        std::unique_ptr<cl::Buffer> mBinCountCL; // CxCxC-sized uint buffer, containing particle count per cell
        std::unique_ptr<cl::Buffer> mBinStartIDCL;
        std::unique_ptr<cl::Buffer> mParticleInBinPosCL;
        std::unique_ptr<cl::Buffer> mParticleCurlsCL;

        std::vector<cl::Memory> mMemObjects;

        std::unique_ptr<cl::Program> mTimestepProgram;
        std::unique_ptr<cl::Program> mPositionAdjustmentProgram;
        std::unique_ptr<cl::Program> mClipToBoundsProgram;
        std::unique_ptr<cl::Program> mCountingSortProgram;

        std::unique_ptr<cl::Kernel> mTimestepKernel;

        std::unique_ptr<cl::Kernel> mSortInsertParticles;
        std::unique_ptr<cl::Kernel> mSortComputeBinStartID;
        std::unique_ptr<cl::Kernel> mSortReindexParticles;

        std::unique_ptr<cl::Kernel> mCalcDensities;
        std::unique_ptr<cl::Kernel> mCalcLambdas;
        std::unique_ptr<cl::Kernel> mCalcDeltaPositionAndDoUpdate;

        std::unique_ptr<cl::Kernel> mRecalcVelocities;
        std::unique_ptr<cl::Kernel> mCalcCurls;
        std::unique_ptr<cl::Kernel> mApplyVortAndViscXSPH;
        std::unique_ptr<cl::Kernel> mSetPositionsFromPredictions;

        std::unique_ptr<cl::Kernel> mClipToBoundsKernel;

        /// FPS

        void updateTimeLabelsInGUI(double timeSinceLastUpdate);

        std::deque<double> mSimulationTimes;

        static const uint NUM_AVG_SIM_TIMES;

        static const uint NUM_MAX_PARTICLES;

        double mTimeOfLastUpdate;
        uint mFramesSinceLastUpdate;

        nanogui::Label *mLabelFPS;
        nanogui::Label *mLabelAverageFrameTime;
    };
}