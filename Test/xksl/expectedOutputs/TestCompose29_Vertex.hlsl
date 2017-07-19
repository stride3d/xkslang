int o0S12C0_TestDirectLightGroup_GetMaxLightCount()
{
    return 1;
}

void o1S2C0_TestMaterialSurfaceArray_Compute()
{
    int res = 0;
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
