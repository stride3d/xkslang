#version 450

int o0S2C0_ShaderComputeColor_Compute(int i)
{
    return i;
}

int main()
{
    int param = 5;
    return o0S2C0_ShaderComputeColor_Compute(param);
}

