#pragma once

#include <glm/glm.hpp>
#include <CL/cl.hpp>

#define STYPE_PLANE 0
#define STYPE_SPHERE 1
#define STYPE_BOX 2

#define DEFAULT_MASS 1.0f

namespace clgl {
    struct __attribute__((packed)) Plane {
        Plane(const glm::vec3 &position, const glm::vec3 &normal);

        Plane(const cl_float3 &position, const cl_float3 &normal);

        cl_float3 normal;
        cl_float3 position;
    };

    struct __attribute__((packed)) Sphere {
        Sphere(const glm::vec3 &position, cl_float radius, cl_float mass = DEFAULT_MASS);

        Sphere(const cl_float3 &position, cl_float radius, cl_float mass = DEFAULT_MASS);

        cl_float mass;
        cl_float radius;

        cl_float3 position;
        cl_float3 velocity;
        cl_float3 impulses;
    };

    struct __attribute__((packed)) Box {
        Box(const glm::vec3 &position, const glm::vec3 &halfDimensions, cl_float mass = DEFAULT_MASS);

        Box(const cl_float3 &position, const cl_float3 &halfDimensions, cl_float mass = DEFAULT_MASS);

        cl_float mass;
        cl_float3 halfDimensions;

        cl_float3 position;
        cl_float3 velocity;
        cl_float3 impulses;
        cl_float3 torques;
    };

    struct __attribute__((packed)) SolidObject {
        cl_uint type;

        union {
            Plane plane;
            Sphere sphere;
            Box box;
        } data;
    };


}

#include "SolidObject.inl"
