#pragma once

#include <string>

namespace util {
    inline std::string convertToCLDefines(const uint nDefines, const std::string args[]) {
        std::stringstream ss;
        for (uint i = 0; i < nDefines; ++i) {
            ss << "#define " << args[2 * i] << " " << args[2 * i + 1] << std::endl;
        }
        return ss.str();
    }
}