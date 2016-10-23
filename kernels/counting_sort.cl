/// Pre-processor defines that specify grid parameters
/// halfDims[Z,Y,Z]         // The dimensions/2 of the grid
/// binSize                 // The side-length of a bin
/// binCount[Z,Y,Z]         // The number of bins in each dimension
/// binCount                // The total number of bins in the grid
/// NO_EDGE_CLAMP

inline uint3 getBinID_3D(const float3 position) {
#ifndef NO_EDGE_CLAMP
    uint3 indices;

//    indices.x = convert_uint_rtn(clamp((position.x + halfDimsX) / binSize, 0.0f, convert_float(binCountX - 1)));
//    indices.y = convert_uint_rtn(clamp((position.y + halfDimsY) / binSize, 0.0f, convert_float(binCountY - 1)));
//    indices.z = convert_uint_rtn(clamp((position.z + halfDimsZ) / binSize, 0.0f, convert_float(binCountZ - 1)));
//    return indices;
    return convert_uint3(clamp(
        floor((position + float3(halfDimsZ,halfDimsY,halfDimsZ)) / binSize),
        float3(0,0,0),
        float3(binCountZ-1, binCountY-1, binCountZ-1)
    ));
#elif
    // ifdef NO_EDGE_CLAMP
    return convert_uint3( floor((position + float3(halfDimsZ,halfDimsY,halfDimsZ))) / binSize) );
#endif
}

inline uint getBinID(const uint3 binID_3D) {
    return binID_3D.x + binCountX * binID_3D.y + binCountX * binCountY * binID_3D.z;
}

__kernel void insert_particles(__global const float3    *positions,
                               __global uint            *particleBinID,
                               __global uint            *particleInBinID,
                               __global uint            *binCounts) {
    const uint id = get_global_id(0);

    // Compute the bin ID of this particle
    const uint binID = getBinID(getBinID_3D(positions[id]));
    particleBinID[id] = binID;

    // Read the current count of particles in the bin and store in this particle's "within-bin-ID"
    // Increment the particle count of the bin
    particleInBinID[id] = atomic_inc(&binCounts[binID]);
}

__kernel void compute_bin_start_ID(__global const uint  *binCounts,
                                   __global uint        *binStartID) {
    const uint id = get_global_id(0);

    uint count = 0;
    for (uint prior_id = 0; prior_id < id; ++prior_id) {
        count += binCounts[prior_id];
    }

    binStartID[id] = count;
}

__kernel void reindex_particles(__global const uint     *particleBinIDsOld,     // 0
                                __global const uint     *particleInBinID,       // 1
                                __global const uint     *binStartID,            // 2
                                __global const float3   *positionsOld,          // 3
                                __global const float3   *velocitiesOld,         // 4
                                __global float3         *positionsNew,          // 5
                                __global float3         *velocitiesNew,         // 6
                                __global uint           *particleBinIDsNew) {   // 7

    const uint id = get_global_id(0);

    const uint idNew = binStartID[particleBinIDsOld[id]] + particleInBinID[id];

    // Copy particle state to new position
    positionsNew[idNew] = positionsOld[id];
    velocitiesNew[idNew] = velocitiesOld[id];
    particleBinIDsNew[idNew] = particleBinIDsOld[id];
}
