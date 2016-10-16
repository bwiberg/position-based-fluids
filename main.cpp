#include <Application.hpp>
#include <util/make_unique.hpp>
#include <iostream>

class HelloScene : public clgl::BaseScene {
public:
    HelloScene(cl::Context &context, cl::CommandQueue &queue) : clgl::BaseScene(context, queue) {
        std::cout << "HelloScene created." << std::endl;
    }
    virtual ~HelloScene() {
        std::cout << "HelloScene destroyed." << std::endl;
    }
    virtual void reset() override {}
    virtual void update(double dt) override {}
    virtual void render() override {}
};

class WorldScene : public clgl::BaseScene {
public:
    WorldScene(cl::Context &context, cl::CommandQueue &queue) : clgl::BaseScene(context, queue) {
        std::cout << "WorldScene created." << std::endl;
    }
    virtual ~WorldScene() {
        std::cout << "WorldScene destroyed." << std::endl;
    }
    virtual void reset() override {}
    virtual void update(double dt) override {}
    virtual void render() override {}
};

#define ADD_SCENE(name,class) clgl::Application::addSceneCreator(name, [] (cl::Context &context, cl::CommandQueue &queue) { return util::make_unique<class>(context, queue); });

int main(int argc, char *argv[]) {
    ADD_SCENE("Hello Scene 2", HelloScene);
    ADD_SCENE("World Scene 2", WorldScene);
    clgl::Application app(argc, argv);

    return app.run();
}