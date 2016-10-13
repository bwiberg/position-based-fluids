#pragma once

#include <chrono>
#include <iostream>

using namespace std::chrono;

namespace tictoc {
    bool is_tictocing = false;
    high_resolution_clock::time_point tp_tictoc_start;
}

void tic() {
    if (tictoc::is_tictocing) {
        return;
    }

    tictoc::tp_tictoc_start = high_resolution_clock::now();
    tictoc::is_tictocing = true;
}

void toc() {
    if (!tictoc::is_tictocing) {
        return;
    }

    auto tp_tictoc_end = high_resolution_clock::now();

    auto nanos = duration_cast<nanoseconds>(tp_tictoc_end - tictoc::tp_tictoc_start);
    std::cout << "The elapsed time is " << 1e-6 * nanos.count() << " milliseconds.\n";

    tictoc::is_tictocing = false;
}