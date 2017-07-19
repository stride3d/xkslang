#version 450

int o0S12C0_TestDirectLightGroup_8__GetMaxLightCount()
{
    return 8;
}

int o1S12C0_TestDirectLightGroup_7__GetMaxLightCount()
{
    return 7;
}

void o2S2C0_TestMaterialSurfaceArray_Compute()
{
    int res = 0;
    res += o0S12C0_TestDirectLightGroup_8__GetMaxLightCount();
    res += o1S12C0_TestDirectLightGroup_7__GetMaxLightCount();
}

void main()
{
    o2S2C0_TestMaterialSurfaceArray_Compute();
}

