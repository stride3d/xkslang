#version 450

struct Base_streamBuffer
{
    int ColorTarget;
};

Base_streamBuffer Base_streamBuffer_var;

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
    Base_streamBuffer_var.ColorTarget = C_Compute();
}

