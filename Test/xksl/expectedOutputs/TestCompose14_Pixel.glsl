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

int main()
{
    int param = 1;
    return o0S2C0_CompB_Compute(param);
}

