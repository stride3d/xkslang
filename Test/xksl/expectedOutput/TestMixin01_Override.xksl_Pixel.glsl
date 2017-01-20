#version 450

struct StreamBuffer
{
    int TestMixin01_Base_ColorTarget;
};

StreamBuffer StreamBuffer_1;

int TestMixin01_OverrideB_Compute()
{
    return 5;
}

void main()
{
    StreamBuffer_1.TestMixin01_Base_ColorTarget = 1 + TestMixin01_OverrideB_Compute();
}

