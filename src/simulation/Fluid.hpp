#pragma once

namespace pbf {
    struct Fluid {
        static Fluid GetDefault();

        // Particle mass: 1.0kg
        // Kernel radius (h): 0.1m
        cl_float kernelRadius;
        // Rest density (rho): 6378.0kg/m^2
        cl_float restDensity;
        // Density Iterations: 4
        // Time step (dt): 0.0083s (2 substeps of a 60hz frame time)
        cl_float deltaTime;
        // CFM Parameter (epsilon): 600
        cl_float epsilon;
        // Artificial Pressure Strength (s_corr): 0.0001
        cl_float s_corr;
        // Artificial Pressure Radius (delta q): 0.03m
        cl_float delta_q;
        // Artificial Pressure Power (n): 4
        cl_uint n;
        // Artificial Viscosity (c): <= 0.01"
        cl_float c;
    };

    inline Fluid Fluid::GetDefault() {
        Fluid fluid;

        fluid.kernelRadius  = 0.1f;
        fluid.restDensity   = 6378f;
        fluid.deltaTime     = 0.0083f;
        fluid.epsilon       = 600f;
        fluid.s_corr        = 0.0001f;
        fluid.delta_q       = 0.03f;
        fluid.n             = 4;
        fluid.c             = 0.01f;

        return fluid;
    }
}