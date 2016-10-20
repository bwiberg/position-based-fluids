#pragma once

#include <CL/cl.hpp>

namespace pbf {
    struct Bounds {
        cl_float3 dimensions;
        cl_float3 halfDimensions;
    };
}