typedef struct def_Grid {
    float3 halfDimensions;

    // The side-length of a bin
    float binSize;

    // The number of bins in each dimension
    uint3 binCount3D;

    // The total number of bins in the grid
    uint binCount;
} Bounds;
