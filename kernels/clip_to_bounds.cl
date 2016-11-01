typedef struct def_Bounds {
	float3 dimensions;
	float3 halfDimensions;
} Bounds;

#define DIFF float3(0.015f, 0.015f, 0.015f)
#define ID get_global_id(0)

__kernel void clip_to_bounds(__global float3* positions,
                             const Bounds bounds) {

    // Clamp the xyz-coordinates to the bounds seperately
    positions[ID] = clamp(positions[ID],
                          -bounds.halfDimensions + DIFF,
                          bounds.halfDimensions - DIFF);
}
