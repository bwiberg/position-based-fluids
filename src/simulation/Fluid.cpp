#include "Fluid.hpp"

namespace pbf {
    std::unique_ptr<Fluid> Fluid::GetDefault() {
        std::unique_ptr<Fluid> fluid = util::make_unique<Fluid>();

        fluid->kernelRadius = 0.1f;
        fluid->restDensity  = 6378.0f;
        fluid->deltaTime    = 0.0083f;
        fluid->epsilon      = 600.0f;
        fluid->k            = 0.0001f;
        fluid->delta_q      = 0.03f;
        fluid->n            = 4;
        fluid->c            = 0.01f;
        fluid->k_vc         = 1.0f;

        fluid->kBoundsDensity = 1.0f;

        return fluid;
    }
}