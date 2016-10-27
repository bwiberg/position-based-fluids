#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

#define ID get_global_id(0)
/// Pre-processor defines that specify grid parameters
/// halfDims[Z,Y,Z]         // The dimensions/2 of the grid
/// binSize                 // The side-length of a bin
/// binCount[Z,Y,Z]         // The number of bins in each dimension
/// binCount                // The total number of bins in the grid
/// NO_EDGE_CLAMP

inline uint3 getBinID_3D(const float3 position) {
#ifndef NO_EDGE_CLAMP
    float3 tmp = (position + float3(halfDimsX, halfDimsY, halfDimsZ)) / binSize;
    uint3 indices = convert_uint3(floor(tmp));
    return clamp(indices, uint3(0, 0, 0), uint3(binCountX-1, binCountY-1, binCountZ-1));
//    return convert_uint3(clamp(
//        floor((position + float3(halfDimsZ,halfDimsY,halfDimsZ)) / binSize),
//        float3(0,0,0),
//        float3(binCountZ-1, binCountY-1, binCountZ-1)
//    ));
#elif
    // ifdef NO_EDGE_CLAMP
    return convert_uint3( floor((position + float3(halfDimsZ,halfDimsY,halfDimsZ))) / binSize) );
#endif
}

inline uint getBinID(const uint3 binID_3D) {
    return binID_3D.x + binCountX * binID_3D.y + binCountX * binCountY * binID_3D.z;
}

__kernel void insert_particles(__global const float3    *positions,
                               __global volatile uint   *particleBinID,
                               __global volatile uint   *particleInBinID,
                               __global volatile uint   *binCounts) {
    // Compute the bin ID of this particle
    const uint binID = getBinID(getBinID_3D(positions[ID]));
    particleBinID[ID] = binID;

    // Read the current count of particles in the bin and store in this particle's "within-bin-ID"
    // Increment the particle count of the bin
    particleInBinID[ID] = atomic_inc(&binCounts[binID]);
}

__kernel void compute_bin_start_ID(__global const uint  *binCounts,
                                   __global uint        *binStartID) {
    uint count = 0;
    for (uint prior_id = 0; prior_id < ID; ++prior_id) {
        count = count + binCounts[prior_id];
    }

    binStartID[ID] = count;
}

__kernel void reindex_particles(__global const uint     *particleBinIDsOld,     // 0
                                __global const uint     *particleInBinID,       // 1
                                __global const uint     *binStartID,            // 2
                                __global const float3   *positionsOld,          // 3
                                __global const float3   *velocitiesOld,         // 4
                                __global float3         *positionsNew,          // 5
                                __global float3         *velocitiesNew,         // 6
                                __global uint           *particleBinIDsNew) {   // 7

    const uint idNew = binStartID[particleBinIDsOld[ID]] + particleInBinID[ID];

    // Copy particle state to new position
    positionsNew[idNew] = positionsOld[ID];
    velocitiesNew[idNew] = velocitiesOld[ID];
    particleBinIDsNew[idNew] = particleBinIDsOld[ID];
}
