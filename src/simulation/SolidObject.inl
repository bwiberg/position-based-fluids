namespace clgl {
    inline Plane::Plane(const glm::vec3 &position, const glm::vec3 &normal)
            : position(position.x, position.y, position.z, 0.0f), normal(normal.x, normal.y, normal.z, 0.0f) {}

    inline Plane::Plane(const cl_float3 &position, const cl_float3 &normal)
            : position(position), normal(normal) {}

    inline Sphere::Sphere(const glm::vec3 &position, cl_float radius, cl_float mass)
            : position(position.x, position.y, position.z, 0.0f), radius(radius), mass(mass) {}

    inline Sphere::Sphere(const cl_float3 &position, cl_float radius, cl_float mass)
            : position(position), radius(radius), mass(mass) {}

    inline Box::Box(const glm::vec3 &position, const glm::vec3 &halfDimensions, cl_float mass)
            : position(position.x, position.y, position.z, 0.0f),
              halfDimensions(halfDimensions.x, halfDimensions.y, halfDimensions.z, 0.0f),
              mass(mass) {}

    inline Box::Box(const cl_float3 &position, const cl_float3 &halfDimensions, cl_float mass)
            : position(position), halfDimensions(halfDimensions), mass(mass) {}
}
