__kernel void timestep(__global float3* positions,
                       __global float3* velocities,
                       const float dt) {
    const uint id = get_global_id(0);

    float3 velocity = velocities[id];
    float3 position = positions[id];

    velocity.y -= dt * 9.82f;

    position.x += dt * velocity.x;
    position.y += dt * velocity.y;
    position.z += dt * velocity.z;

    velocities[id] = velocity;
    positions[id] = position;
}