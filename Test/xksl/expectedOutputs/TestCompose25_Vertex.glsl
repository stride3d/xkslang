#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform PreView
{
    int o0S12C0_TestDirectLightGroup_aVar1;
    int o1S12C0_TestDirectLightGroup_aVar1;
    int o2S2C0_TestMaterialSurfaceArray_aVar2;
} PreView_var;

int o0S12C0_TestDirectLightGroup_GetMaxLightCount()
{
    return PreView_var.o0S12C0_TestDirectLightGroup_aVar1;
}

int o1S12C0_TestDirectLightGroup_GetMaxLightCount()
{
    return PreView_var.o1S12C0_TestDirectLightGroup_aVar1;
}

void o2S2C0_TestMaterialSurfaceArray_Compute()
{
    int res = PreView_var.o2S2C0_TestMaterialSurfaceArray_aVar2;
    res += o0S12C0_TestDirectLightGroup_GetMaxLightCount();
    res += o1S12C0_TestDirectLightGroup_GetMaxLightCount();
}

void main()
{
    o2S2C0_TestMaterialSurfaceArray_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}

