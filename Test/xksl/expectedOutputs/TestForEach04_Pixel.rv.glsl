#version 450

int o1S2C1_ComputeColorA_Compute(int i)
{
    return -i;
}

int main()
{
    int res = 0;
    int param = res;
    res += o1S2C1_ComputeColorA_Compute(param);
    return res;
}

