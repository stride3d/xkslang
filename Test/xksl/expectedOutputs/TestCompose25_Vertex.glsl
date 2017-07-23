#version 450

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
}

