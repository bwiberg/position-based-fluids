#include "Application.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace clgl {
    Application::Application(int argc, char *argv[]) {
        std::vector<std::string> args;

        for (unsigned int argn = 0; argn < argc; ++argn) {
            args.push_back(std::string(argv[argn]));
        }

        for (auto arg : args) {
            std::cout << arg << std::endl;
        }
    }

    void Application::run() {
        std::cout << "Running application!" << std::endl;
    }
}
