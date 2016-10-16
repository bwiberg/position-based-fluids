__kernel void timestep(__global float3* positions,
                       __global float3* velocities,
                       const float dt) {
    positions[get_global_id(0)] = positions[get_global_id(0)] + velocities[get_global_id(0)] * dt;
}