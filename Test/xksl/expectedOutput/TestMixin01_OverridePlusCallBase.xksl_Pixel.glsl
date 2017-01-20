#version 450

struct StreamBuffer
{
    int TestMixin01_Base_ColorTarget;
};

StreamBuffer StreamBuffer_1;

int TestMixin01_Base_Compute()
{
    return 1;
}

int TestMixin01_OverridePlusCallBase_Compute()
{
    return TestMixin01_Base_Compute() + 3;
}

void main()
{
    StreamBuffer_1.TestMixin01_Base_ColorTarget = TestMixin01_OverridePlusCallBase_Compute();
}

