int o0S12C0_TestDirectLightGroup_Compute()
{
    return 1;
}

int o1S12C0_TestDirectLightGroup_Compute()
{
    return 1;
}

void vert_main()
{
    int res;
    res += o0S12C0_TestDirectLightGroup_Compute();
    res += o1S12C0_TestDirectLightGroup_Compute();
}

void main()
{
    vert_main();
}
