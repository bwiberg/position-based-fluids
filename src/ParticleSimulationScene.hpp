#pragma once

#include "BaseScene.hpp"

#include <bwgl/bwgl.hpp>

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

    private:
        double mDeltaTime;

        bwgl::Shader mShader;

        std::unique_ptr<bwgl::VertexBuffer> mPositionsGL;
        std::unique_ptr<bwgl::VertexBuffer> mVelocitiesGL;
        std::unique_ptr<bwgl::VertexBuffer> mDensitiesGL;

        std::unique_ptr<bwgl::VertexArray> mParticles;

        std::unique_ptr<cl::BufferGL> mPositionsCL;
        std::unique_ptr<cl::BufferGL> mVelocitiesCL;
        std::unique_ptr<cl::BufferGL> mDensitiesCL;

        std::unique_ptr<cl::Program> mTimestepProgram;
        std::unique_ptr<cl::KernelFunctor> mTimestepFunctor;
    };
}