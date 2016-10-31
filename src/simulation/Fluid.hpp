#pragma once

#include <memory>
#include <CL/cl.hpp>
#include "util/make_unique.hpp"

namespace pbf {
    struct Fluid {
        static std::unique_ptr<Fluid> GetDefault();

        static void ReadFromFile(const std::string &filename, Fluid &fluid);

        static void WriteToFile(const std::string &filename, const Fluid &fluid);

        // Particle mass: 1.0kg
        // Kernel radius (h): 0.1m
        cl_float kernelRadius;

        cl_uint numSubSteps;
        // Rest density (rho): 6378.0kg/m^2
        cl_float restDensity;
        // Density Iterations: 4
        // Time step (dt): 0.0083s (2 substeps of a 60hz frame time)
        cl_float deltaTime;
        // CFM Parameter (epsilon): 600
        cl_float epsilon;
        // Artificial Pressure Strength (k): 0.0001
        cl_float k;
        // Artificial Pressure Radius (delta q): 0.03m
        cl_float delta_q;
        // Artificial Pressure Power (n): 4
        cl_uint n;
        // Artificial Viscosity (c): <= 0.01"
        cl_float c;
        // Vorticity Confinement strength (k_vc)
        cl_float k_vc;

        cl_float kBoundsDensity;
    };
}