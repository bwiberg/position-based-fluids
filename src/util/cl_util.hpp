#pragma once

#include <string>
#include <memory>
#include <CL/cl.hpp>
#include <bwgl/bwgl.hpp>
#include "OCL_CALL.hpp"
#include "paths.hpp"
#include "make_unique.hpp"

namespace util {
    inline std::string ConvertToCLDefines(const uint nDefines,
                                          const std::string *args) {
        std::stringstream ss;
        for (uint i = 0; i < nDefines; ++i) {
            ss << "#define " << args[2 * i] << " " << args[2 * i + 1] << std::endl;
        }
        return ss.str();
    }

    inline std::unique_ptr<cl::Program> LoadCLProgram(const std::string &kernelName,
                                                      cl::Context &context,
                                                      cl::Device &device,
                                                      const std::string &prefix = "") {

        std::unique_ptr<cl::Program> program = nullptr;
        std::string kernelSource = "";
        if (bwgl::TryReadFromFile(KERNELPATH(kernelName), kernelSource)) {
            OCL_ERROR;
            OCL_CHECK(program = make_unique<cl::Program>(context,
                                                         prefix + "\n" + kernelSource,
                                                         true,
                                                         CL_ERROR));
            if (*CL_ERROR == CL_BUILD_PROGRAM_FAILURE) {
                std::cerr << "Error building: "
                          << program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)
                          << std::endl;

                program = nullptr;
            }
        }
        return program;
    }
}