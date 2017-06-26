#version 450

struct PS_STREAMS
{
    int ColorTarget_id0;
};

layout(location = 0) out int PS_OUT_ColorTarget;

int OverrideB_Compute()
{
    return 5;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0);
    _streams.ColorTarget_id0 = 1 + OverrideB_Compute();
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
}

