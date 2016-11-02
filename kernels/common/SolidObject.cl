#define STYPE_PLANE 0
#define STYPE_SPHERE 1
#define STYPE_BOX 2

typedef struct def_Plane {
    float3 normal;
    float3 position;
} Plane;

typedef struct def_Sphere {
    float mass;
    float radius;

    float3 position;
    float3 velocity;
    float3 impulses;
} Sphere;

typedef struct def_Box {
    float mass;
    float3 halfDimensions;

    float3 position;
    float3 velocity;
    float3 impulses;
    float3 torques;
} Box;

typedef struct def_SolidObject {
    uint type;

    union {
        Plane plane;
        Sphere sphere;
        Box box;
    } data;
} SolidObject;
