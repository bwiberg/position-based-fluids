#include <Application.hpp>
#include <ParticleSimulationScene.hpp>
#include <util/make_unique.hpp>

#define ADD_SCENE(name, class) clgl::Application::addSceneCreator(name, [] (cl::Context &context, cl::Device &device, cl::CommandQueue &queue) { return util::make_unique<class>(context, device, queue); });

int main(int argc, char *argv[]) {
    ADD_SCENE("Fluid Simulation Scene", pbf::ParticleSimulationScene);
    clgl::Application app(argc, argv);
    return app.run();
}