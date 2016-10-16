#include <Application.hpp>

#define ADD_SCENE(name, class) clgl::Application::addSceneCreator(name, [] (cl::Context &context, cl::Device &device, cl::CommandQueue &queue) { return util::make_unique<class>(context, device, queue); });

int main(int argc, char *argv[]) {
    clgl::Application app(argc, argv);
    return app.run();
}