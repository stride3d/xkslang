#version 450

struct PS_STREAMS
{
    int ColorTarget_id0;
};

layout(location = 0) out int PS_OUT_ColorTarget;

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
    PS_STREAMS _streams = PS_STREAMS(0);
    _streams.ColorTarget_id0 = C_Compute();
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
}

