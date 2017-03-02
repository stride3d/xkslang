#version 450

int Utils_Negate(int i)
{
    return -i;
}

int o0S2C0_CompB_Compute(int i)
{
    int param = i;
    return Utils_Negate(param);
}

int ShaderA_main()
{
    int param = 1;
    return o0S2C0_CompB_Compute(param);
}

int Utils_Add(int i, int k)
{
    return i + k;
}

int main()
{
    int i = ShaderA_main();
    int param = 2;
    int param_1 = i;
    int param_2 = 5;
    return o0S2C0_CompB_Compute(param) + Utils_Add(param_1, param_2);
}

