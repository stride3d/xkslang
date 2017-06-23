int o0S2C0_ShaderComputeColor_Compute(int i)
{
    return i;
}

int frag_main()
{
    int param = 5;
    return o0S2C0_ShaderComputeColor_Compute(param);
}

void main()
{
    frag_main();
}
