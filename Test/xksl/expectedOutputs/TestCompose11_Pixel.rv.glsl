#version 450

int o0S2C0_Comp1_Compute(int i, int j)
{
    return i + j;
}

int o0S2C0_Comp1_Compute(int i)
{
    return i;
}

int o0S2C0_Comp1_Compute()
{
    return 10;
}

int o1S2C1_Comp2_Compute(int i, int j)
{
    return i * j;
}

int o1S2C1_Comp2_Compute(int i)
{
    return -i;
}

int o1S2C1_Comp2_Compute()
{
    return 20;
}

int main()
{
    int param = 0;
    int param_1 = 1;
    int param_2 = 2;
    int i1 = (o0S2C0_Comp1_Compute(param, param_1) + o0S2C0_Comp1_Compute(param_2)) + o0S2C0_Comp1_Compute();
    int param_3 = 0;
    int param_4 = 1;
    int param_5 = 2;
    int i2 = (o1S2C1_Comp2_Compute(param_3, param_4) + o1S2C1_Comp2_Compute(param_5)) + o1S2C1_Comp2_Compute();
    return i1 + i2;
}

