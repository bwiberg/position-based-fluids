inline float euclidean_distance2(const float3 r) {
    return r.x * r.x + r.y * r.y + r.z * r.z;
}

inline float euclidean_distance(const float3 r) {
    return sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
}

inline float Wpoly6(const float3 r, const float h) {
    const float tmp = h * h - euclidean_distance2(r);
    if (tmp < EPSILON) {
        return 0.0f;
    }

    return (315.0f / (64.0f * PI * pow(h, 9))) * pow((tmp), 3);
}

inline float3 grad_Wspiky(const float3 r, const float h) {
    const float radius2 = euclidean_distance2(r);
    if (radius2 >= h * h) {
        return float3(0.0f, 0.0f, 0.0f);
    }
    if (radius2 <= EPSILON) {
        return float3(0.0f, 0.0f, 0.0f);
    }

    const float radius = sqrt(radius2);
    const float kernel_constant = -(15 / (PI * pow(h, 6))) * 3 * pow(h - radius, 2) / radius;

    return float3(kernel_constant * r.x,
                  kernel_constant * r.y,
                  kernel_constant * r.z);
}
