#version 450

int TestMixin01_Base_Compute()
{
    return 1;
}

int TestMixin01_A_Compute()
{
    return TestMixin01_Base_Compute() + 2;
}

int main()
{
    return TestMixin01_A_Compute();
}

