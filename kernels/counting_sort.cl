#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

#define ID get_global_id(0)
/// Pre-processor defines that specify grid parameters
/// halfDims[Z,Y,Z]         // The dimensions/2 of the grid
/// binSize                 // The side-length of a bin
/// binCount[Z,Y,Z]         // The number of bins in each dimension
/// binCount                // The total number of bins in the grid

/**
 * Calculates a corresponding 3D-index in the uniform grid based on a position. ("hashing")
 * @param position The position
 * @return The 3D-index
 */
inline uint3 getBinID_3D(const float3 position) {
    float3 tmp = (position + float3(halfDimsX, halfDimsY, halfDimsZ)) / binSize;
    uint3 indices = convert_uint3(floor(tmp));
    return clamp(indices, uint3(0, 0, 0), uint3(binCountX-1, binCountY-1, binCountZ-1));
}

/**
 * Computes the 1D-index into the linearized uniform grid arrays from a 3D-index.
 * @param binID_3D The 3D-index
 * @return The 1D-index
 */
inline uint getBinID(const uint3 binID_3D) {
    return binID_3D.x + binCountX * binID_3D.y + binCountX * binCountY * binID_3D.z;
}

/**
 * Inserts a particle in the grid and increments corresponding counters.
 */
__kernel void insert_particles(__global const float3    *predictedPositions,
                               __global volatile uint   *particleBinID,
                               __global volatile uint   *particleInBinID,
                               __global volatile uint   *binCounts) {
    // Compute the 1D bin index of this particle
    const uint binID = getBinID(getBinID_3D(predictedPositions[ID]));

    // Store the bin index in the particle data
    particleBinID[ID] = binID;

    // Read the current count of particles in the bin and store in this particle's "within-bin-ID", which
    // will be used for actually sorting the particles in the next step.
    // Also atomically increment the particle count of the bin, since this kernel is run in parallel for
    // every particle at the same time.
    particleInBinID[ID] = atomic_inc(&binCounts[binID]);
}

/**
 * Calculates the starting index into the new particle array for a
 * bin, using prefix sum of bin counts.
 */
__kernel void compute_bin_start_ID(__global const uint  *binCounts,
                                   __global uint        *binStartID) {
    uint count = 0;
    for (uint prior_id = 0; prior_id < ID; ++prior_id) {
        // Increment this bin's starting index with the count of a previous bin
        count = count + binCounts[prior_id];
    }

    binStartID[ID] = count;
}

/**
 * Copy the particle state of an unsorted particle to its new, sorted, index in
 * the new particle state arrays.
 */
__kernel void reindex_particles(__global const uint     *particleInBinID,       // 0
                                __global const uint     *binStartID,            // 1

                                __global const float3   *previousPositionsOld,  // 2
                                __global const float3   *predictedPositionsOld, // 3
                                __global const float3   *velocitiesOld,         // 4
                                __global const uint     *particleBinIDsOld,     // 5

                                __global float3         *previousPositionsNew,  // 6
                                __global float3         *predictedPositionsNew, // 7
                                __global float3         *velocitiesNew,         // 8
                                __global uint           *particleBinIDsNew) {   // 9

    // Compute the new index
    const uint idNew = binStartID[particleBinIDsOld[ID]] + particleInBinID[ID];

    // Copy particle state to new index
    previousPositionsNew[idNew] = previousPositionsOld[ID];
    predictedPositionsNew[idNew] = predictedPositionsOld[ID];
    velocitiesNew[idNew] = velocitiesOld[ID];
    particleBinIDsNew[idNew] = particleBinIDsOld[ID];
}
