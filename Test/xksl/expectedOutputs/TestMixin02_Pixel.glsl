#version 450

struct globalStreams
{
    int ColorTarget_s0;
};

globalStreams globalStreams_var;

int OverrideB_Compute()
{
    return 5;
}

void main()
{
    globalStreams_var.ColorTarget_s0 = 1 + OverrideB_Compute();
}

