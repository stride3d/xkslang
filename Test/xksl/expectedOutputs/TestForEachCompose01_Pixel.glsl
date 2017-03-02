#version 450

int o0S2C0_ComputeColorA_Compute(int i)
{
    return i + 1;
}

int o1S2C0_ComputeColor_Compute(int i)
{
    return i;
}

int o1S2C0_ComputeColorB_Compute(int i)
{
    int param = i;
    return o1S2C0_ComputeColor_Compute(param) + i;
}

int main()
{
    int res = 0;
    int param = res + 1;
    res += o0S2C0_ComputeColorA_Compute(param);
    param = res + 1;
    res += o1S2C0_ComputeColorB_Compute(param);
    return res;
}

