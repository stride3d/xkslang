cbuffer Globals
{
    float DirectLightGroupArray_varf;
};

float o0S2C0_IMaterialSurface_Compute()
{
    return DirectLightGroupArray_varf;
}

void vert_main()
{
    float f = DirectLightGroupArray_varf;
    f += o0S2C0_IMaterialSurface_Compute();
}

void main()
{
    vert_main();
}
