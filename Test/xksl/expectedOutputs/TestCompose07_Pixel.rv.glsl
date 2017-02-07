#version 450

int compS31C0_SubComp_Compute()
{
    return 2;
}

int compS8C0_Comp_Compute()
{
    return 3 + compS31C0_SubComp_Compute();
}

int main()
{
    return compS8C0_Comp_Compute();
}

