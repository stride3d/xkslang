#version 450

int cS2C0_cS2C0_SubComp_Compute()
{
    return 2;
}

int cS2C0_Comp_Compute()
{
    return 3 + cS2C0_cS2C0_SubComp_Compute();
}

int main()
{
    return cS2C0_Comp_Compute();
}

