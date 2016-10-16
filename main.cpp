#include <Application.hpp>

#define ADD_SCENE(name, class) clgl::Application::addSceneCreator(name, [] (cl::Context &context, cl::CommandQueue &queue) { return util::make_unique<class>(context, queue); });

int main(int argc, char *argv[]) {
    clgl::Application app(argc, argv);
    return app.run();
}