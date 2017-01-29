#version 450

int TestMixin01_Base_Compute()
{
    return 1;
}

int TestMixin01_A_Compute()
{
    return TestMixin01_Base_Compute() + 2;
}

int TestMixin01_B_Compute()
{
    return TestMixin01_A_Compute() + 3;
}

int TestMixin01_C_Compute()
{
    return TestMixin01_B_Compute() + 4;
}

int main()
{
    return TestMixin01_C_Compute();
}

