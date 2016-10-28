#define ID get_global_id(0)

__kernel void timestep(__global const float3 *positions,          // 0
                       __global float3       *predictedPositions, // 1
                       __global const float3 *velocities,         // 2
                       const float           dt) {                // 3
    float3 velocity = velocities[ID];
    velocity.y = velocity.y - dt * 9.82f;

    predictedPositions[ID] = positions[ID] + dt * velocity;
}