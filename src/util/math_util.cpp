#include "math_util.hpp"
#include "make_unique.hpp"

namespace util {
    std::random_device random_device;

    std::vector<float> generate_uniform_floats(unsigned int N,
                                               float lower_bound_inclusive,
                                               float upper_bound_exclusive) {
        std::vector<float> randoms(N);

        std::mt19937 mt(random_device());

        if (lower_bound_inclusive < 0.0f) {
            std::uniform_real_distribution<float> distribution(0.0f,
                                                               upper_bound_exclusive -
                                                               lower_bound_inclusive);
            int counter = 0;
            while (counter < N) {
                randoms[counter] = distribution(mt) + lower_bound_inclusive;
                ++counter;
            }

        } else {
            std::uniform_real_distribution<float> distribution(lower_bound_inclusive,
                                                               upper_bound_exclusive);
            int counter = 0;
            while (counter < N) {
                randoms[counter] = distribution(mt) + lower_bound_inclusive;
                ++counter;
            }
        }

        return randoms;
    }

    std::vector<glm::vec3> generate_uniform_vec3s(unsigned int N,
                                                  float x_lower_bound_inclusive,
                                                  float x_upper_bound_exclusive,
                                                  float y_lower_bound_inclusive,
                                                  float y_upper_bound_eyclusive,
                                                  float z_lower_bound_inclusive,
                                                  float z_upper_bound_ezclusive) {
        std::vector<glm::vec3> randoms(N);

        const std::vector<float> random_x = generate_uniform_floats(N, x_lower_bound_inclusive,
                                                                    x_upper_bound_exclusive);
        const std::vector<float> random_y = generate_uniform_floats(N, y_lower_bound_inclusive,
                                                                    y_upper_bound_eyclusive);
        const std::vector<float> random_z = generate_uniform_floats(N, z_lower_bound_inclusive,
                                                                    z_upper_bound_ezclusive);

        int counter = 0;
        while (counter < N) {
            randoms[counter] = glm::vec3(random_x[counter], random_y[counter], random_z[counter]);
            ++counter;
        }

        return randoms;
    }

    std::vector<float> generate_linear_floats(unsigned int N,
                                              float lower_bound_inclusive,
                                              float upper_bound_inclusive) {
        std::vector<float> linears(N);

        const float range = upper_bound_inclusive - lower_bound_inclusive;
        const float delta_range = range / N;

        for (unsigned int i = 0; i < N; ++i) {
            linears[i] = lower_bound_inclusive + delta_range * i;
        }
    }

    std::vector<glm::vec3> generate_linear_vec3s(unsigned int N,
                                                 float x_lower_bound_inclusive,
                                                 float x_upper_bound_inclusive,
                                                 float y_lower_bound_inclusive,
                                                 float y_upper_bound_inclusive,
                                                 float z_lower_bound_inclusive,
                                                 float z_upper_bound_inclusive) {
        std::vector<glm::vec3> linears(N);

        const int N_dim = static_cast<int>(ceil(pow(N, 1.0f / 3)));

        const float range_x = x_upper_bound_inclusive - x_lower_bound_inclusive;
        const float range_y = y_upper_bound_inclusive - y_lower_bound_inclusive;
        const float range_z = z_upper_bound_inclusive - z_lower_bound_inclusive;

        const float delta_range_x = range_x / N_dim;
        const float delta_range_y = range_y / N_dim;
        const float delta_range_z = range_z / N_dim;

        int counter = 0;
        for (unsigned int ix = 0; ix < N_dim; ++ix) {
            for (unsigned iy = 0; iy < N_dim; ++iy) {
                for (unsigned int iz = 0; iz < N_dim; ++iz) {
                    linears[counter++] = glm::vec3(
                            x_lower_bound_inclusive + ix * delta_range_x,
                            y_lower_bound_inclusive + iy * delta_range_y,
                            z_lower_bound_inclusive + iz * delta_range_z);

                    if (counter >= N) {
                        return linears;
                    }
                }
            }
        }

        return linears;
    }
}