typedef struct def_Fluid {
    float kernelRadius;
    uint numSubSteps;
    float restDensity;
    float deltaTime;
    float epsilon;
    float k;
    float delta_q;
    uint n;
    float c;
} Fluid;
