#include <Application.hpp>
#include <util/make_unique.hpp>
#include <iostream>

class HelloScene : public clgl::BaseScene {
public:
    HelloScene() {
        std::cout << "HelloScene created." << std::endl;
    }
    virtual ~HelloScene() {
        std::cout << "HelloScene destroyed." << std::endl;
    }
    virtual void reset() override {}
    virtual void update(double dt) override {}
    virtual void render(double dt) override {}
};

class WorldScene : public clgl::BaseScene {
public:
    WorldScene() {
        std::cout << "WorldScene created." << std::endl;
    }
    virtual ~WorldScene() {
        std::cout << "WorldScene destroyed." << std::endl;
    }
    virtual void reset() override {}
    virtual void update(double dt) override {}
    virtual void render(double dt) override {}
};

int main(int argc, char *argv[]) {
    clgl::Application::addSceneCreator("Hello Scene", [] { return util::make_unique<HelloScene>(); });
    clgl::Application::addSceneCreator("World Scene", [] { return util::make_unique<WorldScene>(); });

    clgl::Application app(argc, argv);

    return app.run();
}