cbuffer PreView
{
    int o0S12C0_TestDirectLightGroup_aVar1;
    int o1S2C0_TestMaterialSurfaceArray_aVar2;
};

int o0S12C0_TestDirectLightGroup_GetMaxLightCount()
{
    return o0S12C0_TestDirectLightGroup_aVar1;
}

void o1S2C0_TestMaterialSurfaceArray_Compute()
{
    int res = o1S2C0_TestMaterialSurfaceArray_aVar2;
    res += o0S12C0_TestDirectLightGroup_GetMaxLightCount();
}

void vert_main()
{
    o1S2C0_TestMaterialSurfaceArray_Compute();
}

void main()
{
    vert_main();
}
