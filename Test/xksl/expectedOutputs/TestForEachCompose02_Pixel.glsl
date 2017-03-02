#version 450

int o2S2C1_ComputeColorA_Compute(int i)
{
    return 1;
}

int o0S2C0_ComputeColorA_Compute(int i)
{
    return 1;
}

int o3S2C1_ComputeColorB_Compute(int i)
{
    return 2;
}

int o1S2C0_ComputeColorB_Compute(int i)
{
    return 2;
}

int main()
{
    int res = 0;
    int param = res;
    int param_1 = o2S2C1_ComputeColorA_Compute(param);
    res = o0S2C0_ComputeColorA_Compute(param_1);
    param = res;
    param_1 = o3S2C1_ComputeColorB_Compute(param);
    res = o0S2C0_ComputeColorA_Compute(param_1);
    param = res;
    param_1 = o2S2C1_ComputeColorA_Compute(param);
    res = o1S2C0_ComputeColorB_Compute(param_1);
    param = res;
    param_1 = o3S2C1_ComputeColorB_Compute(param);
    res = o1S2C0_ComputeColorB_Compute(param_1);
    return res;
}

