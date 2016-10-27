/// Pre-processor defines that specify grid parameters
/// halfDims[Z,Y,Z]         // The dimensions/2 of the grid
/// binSize                 // The side-length of a bin
/// binCount[Z,Y,Z]         // The number of bins in each dimension
/// binCount                // The total number of bins in the grid
/// NO_EDGE_CLAMP

#define EPSILON 0.0001f
#define PI 3.1415926535f
#define ID get_global_id(0)

typedef struct def_Fluid {
    float kernelRadius;
    float restDensity;
    float deltaTime;
    float epsilon;
    float s_corr;
    float delta_q;
    uint n;
    float c;
} Fluid;

uint3 getBinID_3D(uint binID);

uint getBinID(const uint3 binID_3D);

float euclidean_distance2(const float3 r);

float euclidean_distance(const float3 r);

float Wpoly6(const float3 r, const float h);

float3 grad_Wspiky(const float3 r, const float h);

__kernel void calc_densities(         const Fluid   fluid,          // 0
                             __global const float3  *positions,     // 1
                             __global const uint    *binIDs,        // 2
                             __global const uint    *binStartIDs,   // 3
                             __global const uint    *binCounts,     // 4
                             __global float         *densities) {   // 5

    float density = 0.0f;
    const float3 position = positions[ID];

    const uint binID = binIDs[ID];
    const int3 binID3D = convert_int3(getBinID_3D(binID));

    uint neighbouringBinIDs[3 * 3 * 3];
    uint neighbouringBinCount = 0;

    uint nBinID;
    uint nBinStartID;
    uint nBinCount;
    uint nParticlesInNeighbouring = 0;

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
            ++nParticlesInNeighbouring;
            density = density + Wpoly6(positions[pID] - position, fluid.kernelRadius);
        }

    }

    densities[ID] = density;
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
    return sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
}

inline float Wpoly6(const float3 r, const float h) {
    const float tmp = h * h - euclidean_distance2(r);
    if (tmp < EPSILON) {
        return 0.0f;
    }

    return (315.0f / (64.0f * PI * pow(h, 9))) * pow((tmp), 3);
}
