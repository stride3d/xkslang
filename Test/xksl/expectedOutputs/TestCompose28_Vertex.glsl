#version 450

int o0S12C0_TestDirectLightGroup_GetMaxLightCount()
{
    return 1;
}

int o2S12C0_TestDirectLightGroup_GetMaxLightCount()
{
    return 1;
}

void o1S2C0_TestMaterialSurfaceArray_Compute()
{
    int res = 0;
    res += o0S12C0_TestDirectLightGroup_GetMaxLightCount();
    res += o2S12C0_TestDirectLightGroup_GetMaxLightCount();
}

void main()
{
    o1S2C0_TestMaterialSurfaceArray_Compute();
}

