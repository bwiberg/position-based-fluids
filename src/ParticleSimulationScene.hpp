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

        void initializeParticleStates(std::vector<glm::vec4> && positions,
                                      std::vector<glm::vec4> && velocities,
                                      std::vector<float> && densities);

        std::shared_ptr<clgl::Camera> mCamera;

        std::shared_ptr<clgl::SceneObject> mCameraRotator;

        std::shared_ptr<clgl::MeshObject> mBoundingBox;
        std::unique_ptr<pbf::Bounds> mBoundsCL;

        std::shared_ptr<clgl::DirectionalLight> mDirLight;
        std::shared_ptr<clgl::AmbientLight> mAmbLight;
        std::shared_ptr<clgl::PointLight> mPointLight;

        bool mIsRotatingCamera;

        float mDeltaTime;

        int mNumParticles;

        std::unique_ptr<clgl::BaseShader> mParticlesShader;
        std::shared_ptr<clgl::BaseShader> mBoxShader;

        std::unique_ptr<bwgl::VertexBuffer> mPositionsGL;
        std::unique_ptr<bwgl::VertexBuffer> mVelocitiesGL;
        std::unique_ptr<bwgl::VertexBuffer> mDensitiesGL;

        std::unique_ptr<bwgl::VertexArray> mParticles;

        std::unique_ptr<cl::BufferGL> mPositionsCL;
        std::unique_ptr<cl::BufferGL> mVelocitiesCL;
        std::unique_ptr<cl::BufferGL> mDensitiesCL;

        std::vector<cl::Memory> mMemObjects;

        std::unique_ptr<cl::Program> mTimestepProgram;
        std::unique_ptr<cl::Kernel> mTimestepKernel;

        std::unique_ptr<cl::Program> mClipToBoundsProgram;
        std::unique_ptr<cl::Kernel> mClipToBoundsKernel;
    };
}