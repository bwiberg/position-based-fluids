#include "Fluid.hpp"

#include <fstream>
#include <iomanip>

namespace pbf {
    std::unique_ptr<Fluid> Fluid::GetDefault() {
        std::unique_ptr<Fluid> fluid = util::make_unique<Fluid>();

        fluid->kernelRadius = 0.1f;
        fluid->numSubSteps = 1;
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

    void Fluid::ReadFromFile(const std::string &filename, Fluid &fluid) {
        std::ifstream ifs(filename.c_str());

        std::string param;
        if (ifs.is_open()) {
            while (!ifs.eof()) {
                ifs >> param;

                if (param == "kernelRadius") ifs >> fluid.kernelRadius;
                if (param == "numSubSteps") ifs >> fluid.numSubSteps;
                if (param == "restDensity") ifs >> fluid.restDensity;
                if (param == "deltaTime") ifs >> fluid.deltaTime;
                if (param == "epsilon") ifs >> fluid.epsilon;
                if (param == "k") ifs >> fluid.k;
                if (param == "delta_q") ifs >> fluid.delta_q;
                if (param == "n") ifs >> fluid.n;
                if (param == "c") ifs >> fluid.c;
                if (param == "k_vc") ifs >> fluid.k_vc;
                if (param == "kBoundsDensity") ifs >> fluid.kBoundsDensity;
            }
        }

        ifs.close();
    }

    void Fluid::WriteToFile(const std::string &filename, const Fluid &fluid) {
        std::ofstream ofs(filename.c_str());

        std::string param;
        if (ofs.is_open()) {
            ofs << std::left << std::setw(15) << "kernelRadius " <<  fluid.kernelRadius << std::endl;
            ofs << std::left << std::setw(15) << "numSubSteps " <<  fluid.numSubSteps << std::endl;
            ofs << std::left << std::setw(15) << "restDensity " <<  fluid.restDensity << std::endl;
            ofs << std::left << std::setw(15) << "deltaTime " <<  fluid.deltaTime << std::endl;
            ofs << std::left << std::setw(15) << "epsilon " <<  fluid.epsilon << std::endl;
            ofs << std::left << std::setw(15) << "k " <<  fluid.k << std::endl;
            ofs << std::left << std::setw(15) << "delta_q " <<  fluid.delta_q << std::endl;
            ofs << std::left << std::setw(15) << "n " <<  fluid.n << std::endl;
            ofs << std::left << std::setw(15) << "c " <<  fluid.c << std::endl;
            ofs << std::left << std::setw(15) << "k_vc " <<  fluid.k_vc << std::endl;
            ofs << std::left << std::setw(15) << "kBoundsDensity " <<  fluid.kBoundsDensity << std::endl;
        }

        ofs.close();
    }
}
