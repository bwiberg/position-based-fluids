#pragma once

#include <string>

#define SHADERPATH(filename) std::string(SHADERS_FOLDER) + std::string(filename)
#define KERNELPATH(filename) std::string(KERNELS_FOLDER) + std::string(filename)
#define RESPATH(filename) std::string(RES_FOLDER) + std::string(filename)