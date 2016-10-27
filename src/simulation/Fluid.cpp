#include "Fluid.hpp"

namespace pbf {
    std::unique_ptr<Fluid> Fluid::GetDefault() {
        std::unique_ptr<Fluid> fluid = util::make_unique<Fluid>();

        fluid->kernelRadius  = 0.1f;
        fluid->restDensity   = 6378.0f;
        fluid->deltaTime     = 0.0083f;
        fluid->epsilon       = 600.0f;
        fluid->s_corr        = 0.0001f;
        fluid->delta_q       = 0.03f;
        fluid->n             = 4;
        fluid->c             = 0.01f;

        return fluid;
    }
}