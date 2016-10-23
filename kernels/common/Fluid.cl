typedef struct def_Fluid {
    float kernelRadius;
    float restDensity;
    float deltaTime;
    float epsilon;
    float s_corr;
    float delta_q;
    uint n;
    float c;
} Fluid;
