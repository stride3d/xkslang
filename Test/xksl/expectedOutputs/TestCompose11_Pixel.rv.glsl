#version 450

int cS2C0_Comp1_Compute(int i, int j)
{
    return i + j;
}

int cS2C0_Comp1_Compute(int i)
{
    return i;
}

int cS2C0_Comp1_Compute()
{
    return 10;
}

int cS2C1_Comp2_Compute(int i, int j)
{
    return i * j;
}

int cS2C1_Comp2_Compute(int i)
{
    return -i;
}

int cS2C1_Comp2_Compute()
{
    return 20;
}

int main()
{
    int param = 0;
    int param_1 = 1;
    int param_2 = 2;
    int i1 = (cS2C0_Comp1_Compute(param, param_1) + cS2C0_Comp1_Compute(param_2)) + cS2C0_Comp1_Compute();
    int param_3 = 0;
    int param_4 = 1;
    int param_5 = 2;
    int i2 = (cS2C1_Comp2_Compute(param_3, param_4) + cS2C1_Comp2_Compute(param_5)) + cS2C1_Comp2_Compute();
    return i1 + i2;
}

