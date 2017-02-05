#version 450

struct Base_streamBuffer
{
    int Base_ColorTarget;
};

Base_streamBuffer Base_streamBuffer_var;

int OverrideB_Compute()
{
    return 5;
}

void main()
{
    Base_streamBuffer_var.Base_ColorTarget = 1 + OverrideB_Compute();
}

