int o0S2C0_CompA_Compute(int i)
{
    return 0;
}

int ShaderA_main()
{
    int param = 1;
    return o0S2C0_CompA_Compute(param);
}

int Utils_Negate(int i)
{
    return -i;
}

int o1S21C0_CompB_Compute(int i)
{
    int param = i;
    return Utils_Negate(param);
}

int frag_main()
{
    int i = ShaderA_main();
    int param = 2;
    return o1S21C0_CompB_Compute(param) + i;
}

void main()
{
    frag_main();
}
