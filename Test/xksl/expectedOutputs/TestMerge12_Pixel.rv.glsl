#version 450

struct Base_streamBuffer
{
    int Base_ColorTarget;
};

Base_streamBuffer var_Base_streamBuffer;

int Base_Compute()
{
    return 1;
}

int A1_Compute()
{
    return Base_Compute() + 2;
}

int B1_Compute()
{
    return A1_Compute() + 3;
}

int A2_Compute()
{
    return B1_Compute() + 20;
}

int B2_Compute()
{
    return A2_Compute() + 30;
}

int C_Compute()
{
    return B2_Compute() + 100;
}

void main()
{
    var_Base_streamBuffer.Base_ColorTarget = C_Compute();
}

