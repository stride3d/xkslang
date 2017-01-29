#version 450

struct TestMixin01_Base_streamBuffer
{
    int TestMixin01_Base_ColorTarget;
};

TestMixin01_Base_streamBuffer var_TestMixin01_Base_streamBuffer;

int TestMixin01_OverrideB_Compute()
{
    return 5;
}

void main()
{
    var_TestMixin01_Base_streamBuffer.TestMixin01_Base_ColorTarget = 1 + TestMixin01_OverrideB_Compute();
}

