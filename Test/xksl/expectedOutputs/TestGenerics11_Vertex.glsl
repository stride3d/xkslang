#version 450

layout(std140) uniform Globals
{
    float DirectLightGroupArray_id0_varf;
} Globals_var;

float o0S2C0_IMaterialSurface_Compute()
{
    return Globals_var.DirectLightGroupArray_id0_varf;
}

void main()
{
    float f = Globals_var.DirectLightGroupArray_id0_varf;
    f += o0S2C0_IMaterialSurface_Compute();
}

