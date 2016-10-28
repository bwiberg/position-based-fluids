/// Pre-processor defines that specify grid parameters
/// halfDims[Z,Y,Z]         // The dimensions/2 of the grid
/// binSize                 // The side-length of a bin
/// binCount[Z,Y,Z]         // The number of bins in each dimension
/// binCount                // The total number of bins in the grid
/// NO_EDGE_CLAMP

//#define USE_FAST_SQRT
#define ONE_OVER_SQRT_OF_3 0.577350f
#define ZERO3F float3(0.0f, 0.0f, 0.0f)
#define EPSILON 0.0001f
#define PI 3.1415926535f
#define ID get_global_id(0)

#define MAX_DELTA_PI float3(0.1f, 0.1f, 0.1f)

typedef struct def_Fluid {
    float kernelRadius;
    float restDensity;
    float deltaTime;
    float epsilon;
    float k;
    float delta_q;
    uint n;
    float c;
    float k_vc;

    float kBoundsDensity;
} Fluid;

typedef struct def_Bounds {
    float3 dimensions;
    float3 halfDimensions;
} Bounds;


uint3 getBinID_3D(uint binID);

uint getBinID(const uint3 binID_3D);

float euclidean_distance2(const float3 r);

float euclidean_distance(const float3 r);

float Wpoly6(const float3 r, const float h);

float3 grad_Wspiky(const float3 r, const float h);

float sqroot(float x__);

float calc_bound_density_contribution(float dx_, float kernelRadius_);

__kernel void calc_densities(         const Fluid   fluid,          // 0
                                      const Bounds  bounds,         // 1
                             __global const float3  *positions,     // 2
                             __global const uint    *binIDs,        // 3
                             __global const uint    *binStartIDs,   // 4
                             __global const uint    *binCounts,     // 5
                             __global float         *densities) {   // 6

    float density = 0.0f;
    const float3 position = positions[ID];

    const uint binID = binIDs[ID];
    const int3 binID3D = convert_int3(getBinID_3D(binID));

    uint neighbouringBinIDs[3 * 3 * 3];
    uint neighbouringBinCount = 0;

    uint nBinID;
    uint nBinStartID;
    uint nBinCount;

    int x, y, z;
    for (int dx = -1; dx < 2; ++dx) {
        x = binID3D.x + dx;
        if (x+1 == clamp(x+1, 1, binCountX)) {
            for (int dy = -1; dy < 2; ++dy) {
                y = binID3D.y + dy;
                if (y+1 == clamp(y+1, 1, binCountY)) {
                    for (int dz = -1; dz < 2; ++dz) {
                        z = binID3D.z + dz;
                        if  (z+1 == clamp(z+1, 1, binCountZ)) {
                            nBinID = x + binCountX * y + binCountX * binCountY * z;
                            //printf("%d + %d * %d + %d * %d * %d = %d\n", x, binCountX, y, binCountX, binCountY, z, nBinID);
                            neighbouringBinIDs[neighbouringBinCount] = nBinID;
                            ++neighbouringBinCount;
                        }
                    }
                }
            }
        }
    }

    for (uint i = 0; i < neighbouringBinCount; ++i) {
        uint nBinID = neighbouringBinIDs[i];

        nBinStartID = binStartIDs[nBinID];
        nBinCount = binCounts[nBinID];

        for (uint pID = nBinStartID; pID < (nBinStartID + nBinCount); ++pID) {
            density = density + Wpoly6(positions[pID] - position, fluid.kernelRadius);
        }

    }

    /// Boundary density contributions
    float b_density = 0.0f;
    // x-left
    b_density = b_density + calc_bound_density_contribution(position.x + bounds.halfDimensions.x, fluid.kernelRadius);
    // x-right
    b_density = b_density + calc_bound_density_contribution(bounds.halfDimensions.x - position.x, fluid.kernelRadius);
    // y-down
    b_density = b_density + calc_bound_density_contribution(position.y + bounds.halfDimensions.y, fluid.kernelRadius);
    // y-up
    b_density = b_density + calc_bound_density_contribution(bounds.halfDimensions.y - position.y, fluid.kernelRadius);
    // z-near
    b_density = b_density + calc_bound_density_contribution(position.z + bounds.halfDimensions.z, fluid.kernelRadius);
    // z-far
    b_density = b_density + calc_bound_density_contribution(bounds.halfDimensions.z - position.z, fluid.kernelRadius);

    densities[ID] = density + fluid.kBoundsDensity * b_density;
}

__kernel void calc_lambdas(const Fluid            fluid,          // 0
                           __global const float3  *positions,     // 1
                           __global const uint    *binIDs,        // 2
                           __global const uint    *binStartIDs,   // 3
                           __global const uint    *binCounts,     // 4
                           __global const float   *densities,     // 5
                           __global float         *lambdas) {     // 6

    const float3 position = positions[ID];
    const float density = densities[ID];
    const float Ci = density / fluid.restDensity - 1;

    const uint binID = binIDs[ID];
    const int3 binID3D = convert_int3(getBinID_3D(binID));

    /// Find all neighbours
    uint neighbouringBinIDs[3 * 3 * 3];
    uint neighbouringBinCount = 0;

    uint nBinID;
    uint nBinStartID;
    uint nBinCount;

    int x, y, z;
    for (int dx = -1; dx < 2; ++dx) {
        x = binID3D.x + dx;
        if (x+1 == clamp(x+1, 1, binCountX)) {
            for (int dy = -1; dy < 2; ++dy) {
                y = binID3D.y + dy;
                if (y+1 == clamp(y+1, 1, binCountY)) {
                    for (int dz = -1; dz < 2; ++dz) {
                        z = binID3D.z + dz;
                        if  (z+1 == clamp(z+1, 1, binCountZ)) {
                            nBinID = x + binCountX * y + binCountX * binCountY * z;
                            neighbouringBinIDs[neighbouringBinCount] = nBinID;
                            ++neighbouringBinCount;
                        }
                    }
                }
            }
        }
    }

    /// Calculate gradient^2 of Ci for all neighbours
    float sumOfSquaredGradients = 0.0f;

    //  Accumulator for the case where k=i, i.e. the sum itself should be squared
    float3 grad_ki = ZERO3F;
    float3 k_position = ZERO3F;

    // temp variable for storing the gradient
    float3 tmp_grad = ZERO3F;

    for (uint i = 0; i < neighbouringBinCount; ++i) {
        uint nBinID = neighbouringBinIDs[i];

        nBinStartID = binStartIDs[nBinID];
        nBinCount = binCounts[nBinID];

        // reset accumulator for k=i
        grad_ki = ZERO3F;

        for (uint pID = nBinStartID; pID < (nBinStartID + nBinCount); ++pID) {
            k_position = positions[pID];
            tmp_grad = grad_Wspiky(position - k_position, fluid.kernelRadius);
            grad_ki += tmp_grad;

            if (pID != ID) {
                // k != i, the squared gradient should also be added directly to the denominator
                sumOfSquaredGradients = sumOfSquaredGradients +
                                            tmp_grad.x * tmp_grad.x +
                                            tmp_grad.y * tmp_grad.y +
                                            tmp_grad.z * tmp_grad.z;
            }
        }
    }

    sumOfSquaredGradients = sumOfSquaredGradients +
                                grad_ki.x * grad_ki.x +
                                grad_ki.y * grad_ki.y +
                                grad_ki.z * grad_ki.z;

    /// Compute lambda_i as (-Ci)/(sum(gradient^2 of Ci) + eps)
    const float lambda = - Ci / ((sumOfSquaredGradients / pow(fluid.restDensity, 2)) + fluid.epsilon);
    lambdas[ID] = lambda;

    //printf("<particle ID=%d density=%f lambda=%f>\n", ID, density, lambda);
}

__kernel void calc_delta_pi_and_update(const Fluid            fluid,          // 0
                                       const Bounds           bounds,         // 1
                                       __global float3        *positions,     // 2
                                       __global const uint    *binIDs,        // 3
                                       __global const uint    *binStartIDs,   // 4
                                       __global const uint    *binCounts,     // 5
                                       __global const float   *densities,     // 6
                                       __global const float   *lambdas) {     // 7

    const float3 position = positions[ID];
    const float density = densities[ID];
    const float lambda = lambdas[ID];

    const uint binID = binIDs[ID];
    const int3 binID3D = convert_int3(getBinID_3D(binID));

    /// Find all neighbours
    uint neighbouringBinIDs[3 * 3 * 3];
    uint neighbouringBinCount = 0;

    uint nBinID;
    uint nBinStartID;
    uint nBinCount;

    float3 delta_pi = ZERO3F;

    int x, y, z;
    for (int dx = -1; dx < 2; ++dx) {
        x = binID3D.x + dx;
        if (x+1 == clamp(x+1, 1, binCountX)) {
            for (int dy = -1; dy < 2; ++dy) {
                y = binID3D.y + dy;
                if (y+1 == clamp(y+1, 1, binCountY)) {
                    for (int dz = -1; dz < 2; ++dz) {
                        z = binID3D.z + dz;
                        if  (z+1 == clamp(z+1, 1, binCountZ)) {
                            nBinID = x + binCountX * y + binCountX * binCountY * z;
                            neighbouringBinIDs[neighbouringBinCount] = nBinID;
                            ++neighbouringBinCount;
                        }
                    }
                }
            }
        }
    }

    float3 k_position = ZERO3F;
    float s_corr = 0.0f;
    for (uint i = 0; i < neighbouringBinCount; ++i) {
        uint nBinID = neighbouringBinIDs[i];

        nBinStartID = binStartIDs[nBinID];
        nBinCount = binCounts[nBinID];

        for (uint pID = nBinStartID; pID < (nBinStartID + nBinCount); ++pID) {
            k_position = positions[pID];
            s_corr = - fluid.k * pow(Wpoly6(position - k_position, fluid.kernelRadius) /
                    Wpoly6(float3(ONE_OVER_SQRT_OF_3 * fluid.delta_q,
                                  ONE_OVER_SQRT_OF_3 * fluid.delta_q,
                                  ONE_OVER_SQRT_OF_3 * fluid.delta_q),
                           fluid.kernelRadius), fluid.n);
            delta_pi = delta_pi + (lambda + lambdas[pID] + s_corr) * grad_Wspiky(position - k_position, fluid.kernelRadius);
        }
    }

    delta_pi = delta_pi / fluid.restDensity;

    // Clip to bounds etc.

    positions[ID] = position + clamp(delta_pi, - MAX_DELTA_PI, MAX_DELTA_PI);
}

__kernel void recalc_velocities(__global const float3 *previousPositions,
                                __global const float3 *currentPositions,
                                __global float3       *velocities,
                                const float           oneOverDt) {
    velocities[ID] = oneOverDt * (currentPositions[ID] - previousPositions[ID]);
}

__kernel void calc_curls(const Fluid            fluid,          // 0
                         __global const uint    *binIDs,        // 1
                         __global const uint    *binStartIDs,   // 2
                         __global const uint    *binCounts,     // 3
                         __global const float3  *positions,     // 4
                         __global const float3  *velocities,    // 5
                         __global float3        *curls) {       // 6
    const float3 position = positions[ID];
    const float3 velocity = velocities[ID];

    const uint binID = binIDs[ID];
    const int3 binID3D = convert_int3(getBinID_3D(binID));

    /// Find all neighbours
    uint neighbouringBinIDs[3 * 3 * 3];
    uint neighbouringBinCount = 0;

    uint nBinID;
    uint nBinStartID;
    uint nBinCount;

    int x, y, z;
    for (int dx = -1; dx < 2; ++dx) {
        x = binID3D.x + dx;
        if (x+1 == clamp(x+1, 1, binCountX)) {
            for (int dy = -1; dy < 2; ++dy) {
                y = binID3D.y + dy;
                if (y+1 == clamp(y+1, 1, binCountY)) {
                    for (int dz = -1; dz < 2; ++dz) {
                        z = binID3D.z + dz;
                        if  (z+1 == clamp(z+1, 1, binCountZ)) {
                            nBinID = x + binCountX * y + binCountX * binCountY * z;
                            neighbouringBinIDs[neighbouringBinCount] = nBinID;
                            ++neighbouringBinCount;
                        }
                    }
                }
            }
        }
    }

    /// Calculate particle curl
    float4 curl = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float3 u = ZERO3F;
    float3 v = ZERO3F;

    for (uint i = 0; i < neighbouringBinCount; ++i) {
        uint nBinID = neighbouringBinIDs[i];

        nBinStartID = binStartIDs[nBinID];
        nBinCount = binCounts[nBinID];

        for (uint pID = nBinStartID; pID < (nBinStartID + nBinCount); ++pID) {
            u = velocity - velocities[pID];
            v = grad_Wspiky(position - positions[pID], fluid.kernelRadius);
            curl += cross(float4(u.x, u.y, u.z, 0.0f), float4(v.x, v.y, v.z, 0.0f));
        }
    }

    curls[ID] = float3(curl.x, curl.y, curl.z);
}

__kernel void apply_vort_and_viscXSPH(const Fluid            fluid,             // 0
                                      __global const uint    *binIDs,           // 1
                                      __global const uint    *binStartIDs,      // 2
                                      __global const uint    *binCounts,        // 3
                                      __global const float3  *positions,        // 4
                                      __global const float   *densities,        // 5
                                      __global const float3  *curls,            // 6
                                      __global const float3  *velocitiesIn,     // 7
                                      __global float3        *velocitiesOut) {  // 8

    const float3 position   = positions[ID];
    const float3 velocity   = velocitiesIn[ID];
    const float density     = densities[ID];
    const float3 curl       = curls[ID];

    const uint binID = binIDs[ID];
    const int3 binID3D = convert_int3(getBinID_3D(binID));

    /// Find all neighbours
    uint neighbouringBinIDs[3 * 3 * 3];
    uint neighbouringBinCount = 0;

    uint nBinID;
    uint nBinStartID;
    uint nBinCount;

    int x, y, z;
    for (int dx = -1; dx < 2; ++dx) {
        x = binID3D.x + dx;
        if (x+1 == clamp(x+1, 1, binCountX)) {
            for (int dy = -1; dy < 2; ++dy) {
                y = binID3D.y + dy;
                if (y+1 == clamp(y+1, 1, binCountY)) {
                    for (int dz = -1; dz < 2; ++dz) {
                        z = binID3D.z + dz;
                        if  (z+1 == clamp(z+1, 1, binCountZ)) {
                            nBinID = x + binCountX * y + binCountX * binCountY * z;
                            neighbouringBinIDs[neighbouringBinCount] = nBinID;
                            ++neighbouringBinCount;
                        }
                    }
                }
            }
        }
    }

    /// Apply Vorticity confinement
    float3 n = ZERO3F; //ŋ

    for (uint i = 0; i < neighbouringBinCount; ++i) {
        uint nBinID = neighbouringBinIDs[i];

        nBinStartID = binStartIDs[nBinID];
        nBinCount = binCounts[nBinID];

        for (uint pID = nBinStartID; pID < (nBinStartID + nBinCount); ++pID) {
            n += (1 / (max(densities[pID], 100.0f))) * length(curls[pID]) * grad_Wspiky(position - positions[pID], fluid.kernelRadius);
        }
    }

    float3 n_hat = n / length(n);

    float4 f_vc = fluid.k_vc * cross(float4(n_hat.x, n_hat.y, n_hat.z, 0.0f),
                                            float4(curl.x,  curl.y,  curl.z, 0.0f));

    if (ID == 0) {
        printf("f_vorticity=[%f, %f, %f]\n", f_vc.x, f_vc.y, f_vc.z);
    }
/// Apply XSPH viscosity smoothing
//    float3 sumWeightedNeighbourVelocities = ZERO3F;
//
//    for (uint i = 0; i < neighbouringBinCount; ++i) {
//        uint nBinID = neighbouringBinIDs[i];
//
//        nBinStartID = binStartIDs[nBinID];
//        nBinCount = binCounts[nBinID];
//
//        for (uint pID = nBinStartID; pID < (nBinStartID + nBinCount); ++pID) {
//            sumWeightedNeighbourVelocities +=
//                     (velocity - velocitiesIn[pID]) * Wpoly6(position - positions[pID], fluid.kernelRadius);
//        }
//    }
//
    /* + fluid.c * sumWeightedNeighbourVelocities*/
    velocitiesOut[ID] = velocity + fluid.deltaTime * float3(f_vc.x, f_vc.y, f_vc.z);
}

__kernel void set_positions_from_predictions(__global const float3 *predictedPositions,
                                             __global float3       *positions) {
    positions[ID] = predictedPositions[ID];
}

/// from http://stackoverflow.com/questions/14845084/how-do-i-convert-a-1d-index-into-a-3d-index?noredirect=1&lq=1
inline uint3 getBinID_3D(uint binID) {
    uint3 binID3D;
    binID3D.z = binID / (binCountX * binCountY);
    binID3D.y = (binID - binID3D.z * binCountX * binCountY) / binCountX;
    binID3D.x = binID - binCountX * (binID3D.y + binCountY * binID3D.z);
    return binID3D;
}

inline uint getBinID(const uint3 id3) {
    const uint binID = id3.x + binCountX * id3.y + binCountX * binCountY * id3.z;
    return binID;
}

inline float euclidean_distance2(const float3 r) {
    return r.x * r.x + r.y * r.y + r.z * r.z;
}

inline float euclidean_distance(const float3 r) {
    return sqroot(r.x * r.x + r.y * r.y + r.z * r.z);
}

inline float Wpoly6(const float3 r, const float h) {
    const float tmp = h * h - euclidean_distance2(r);
    if (tmp < EPSILON) {
        return 0.0f;
    }

    return (315.0f / (64.0f * PI * pow(h, 9))) * pow((tmp), 3);
}

inline float3 grad_Wspiky(const float3 r, const float h) {
    const float radius2 = euclidean_distance2(r);
    if (radius2 >= h * h) {
        return ZERO3F;
    }
    if (radius2 <= EPSILON) {
        return ZERO3F;
    }

    const float radius = sqroot(radius2);
    const float kernel_constant = - (15 / (PI * pow(h, 6))) * 3 * pow(h - radius, 2) / radius;

    return (float3)(kernel_constant * r.x,
                    kernel_constant * r.y,
                    kernel_constant * r.z);
}

inline float sqroot(float x__) {
#ifdef USE_FAST_SQRT
    return half_sqrt(x__);
#else
    return sqrt(x__);
#endif
}

inline float calc_bound_density_contribution(float dx_, float kernelRadius_) {
    if (dx_ > kernelRadius_) {
        return 0.0f;
    }

    if (dx_ <= 0.0f) {
        return (2 * PI / 3);
    }

    return (2 * PI / 3) * pow(kernelRadius_ - dx_, 2) * (kernelRadius_ + dx_);
}