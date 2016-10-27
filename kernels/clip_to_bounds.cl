typedef struct def_Bounds {
	float3 dimensions;
	float3 halfDimensions;
} Bounds;

#define EPSILON 0.000001f
#define ID get_global_id(0)

__kernel void clip_to_bounds(__global float3* positions,
                             __global float3* velocities,
                             const Bounds bounds) {

    const float3 origPosition = positions[ID];

    // Clamp the xyz-coordinates to the bounds seperately
    positions[ID] = clamp(origPosition, -bounds.halfDimensions, bounds.halfDimensions);

    // Get the shift in position, i.e. diff between corrected and original positions
    const float3 diff = positions[ID] - origPosition;
    const float3 didChange = convert_float3(fabs(diff) > EPSILON);

    // Multiply the velocity components with the above sign
    velocities[ID] = velocities[ID] * (1 + 2 * didChange);
}
