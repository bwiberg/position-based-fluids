typedef struct def_Bounds {
	float3 dimensions;
	float3 halfDimensions;
} Bounds;

#define EPSILON 0.000001f

__kernel void clip_to_bounds(__global float3* positions,
                             __global float3* velocities,
                             const Bounds bounds) {
    __private const uint id = get_global_id(0);
    const float3 origPosition = positions[id];

    // Clamp the xyz-coordinates to the bounds seperately
    positions[id] = clamp(origPosition, -bounds.halfDimensions, bounds.halfDimensions);

    // Get the shift in position, i.e. diff between corrected and original positions
    const float3 diff = positions[id] - origPosition;
    const float3 didChange = convert_float3(fabs(diff) > EPSILON);

    // Multiply the velocity components with the above sign
    velocities[id] = velocities[id] * (1 + 2 * didChange);
}
