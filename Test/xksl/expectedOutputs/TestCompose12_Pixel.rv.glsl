#version 450

int cS2C0_ShaderComputeColor_Compute(int i)
{
    return i;
}

int main()
{
    int param = 5;
    return cS2C0_ShaderComputeColor_Compute(param);
}

