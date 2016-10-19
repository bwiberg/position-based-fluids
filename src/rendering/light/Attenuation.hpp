#pragma once

namespace clgl {
    struct Attenuation {
        inline Attenuation(float a_ = 0.0f, float b_ = 0.0f) : a(a_), b(b_) {}

        float a;
        float b;
    };
}